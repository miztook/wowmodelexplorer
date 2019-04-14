#include "stdafx.h"
#include "BlpToKTX.h"
#include "mywow.h"
#include "PVRTextureHeader.h"
#include "PVRTexture.h"
#include "PVRTextureUtilities.h"
#include "TGAImageWriter.h"

using namespace pvrtexture;

const char* g_tempTGAFileName = "temp_rgba8.tga";
const char* g_tempKTXFileName = "temp/temp_rgba8.ktx";

CBlpToKTX::CBlpToKTX( PVRCompressionQuality quality ) : Quality(quality)
{

}

bool CBlpToKTX::convertBlpToKTX( IBLPImage* blp, const char* outputpath )
{
	if (!hasFileExtensionA(outputpath, "ktx"))
	{
		_ASSERT(false);
		return false;
	}

	dimension2du size = blp->getDimension();
	uint64 nPVRPixelType = 0;
	ECOLOR_FORMAT format = blp->getColorFormat();
	switch (format)
	{
	case ECF_DXT1:
		nPVRPixelType = ePVRTPF_BC1;
		break;
	case ECF_DXT3:
		nPVRPixelType = ePVRTPF_BC2;
		break;
	case ECF_DXT5:
		nPVRPixelType = ePVRTPF_BC3;
		break;
	case ECF_A8R8G8B8:
		{
			PixelType PixelTypeBGRA('b','g','r','a',8,8,8,8);
			nPVRPixelType = PixelTypeBGRA.PixelTypeID;
		}
		break;
	default:
		_ASSERT(false);
		return false;
	}

	//!! KTX和PVR不同, mipmap只能在创建纹理时生成，文件本身是不带mipmap的
	uint32_t mipLevels = 1; //blp->getNumMipLevels();		
	bool compressed =  isCompressedFormat(format);
	uint32_t bpp = getBytesPerPixelFromFormat(format);

	CPVRTextureHeader PVRHeader(nPVRPixelType, size.Height, size.Width, 1, mipLevels, 1, 1);
	CPVRTexture* pTexture = NULL;

	uint32_t mipDataSize[16] = {0};
	uint32_t mipDataPitch[16] = {0};
	_ASSERT(mipLevels <= 16);

	uint32_t dataSize = 0;
	for (uint32_t i=0; i<mipLevels; ++i)
	{
		dimension2du mipsize = size.getMipLevelSize(i);
		getImagePitchAndBytes(format, mipsize.Width, mipsize.Height, mipDataPitch[i], mipDataSize[i]);
		dataSize += mipDataSize[i];
	}

	uint32_t offset = 0;
	uint8_t* data = (uint8_t*)Z_AllocateTempMemory(dataSize);
	for (uint32_t i=0; i<mipLevels; ++i)
	{
		dimension2du mipsize = size.getMipLevelSize(i);
		blp->copyMipmapData(i, data + offset, mipDataPitch[i], mipsize.Width, mipsize.Height);

		offset += mipDataSize[i];
	}
	_ASSERT(offset == dataSize);

	pTexture = new CPVRTexture(PVRHeader, data);

	Z_FreeTempMemory(data);

	bool ret = processPvrTexture(pTexture, blp, outputpath);

	delete pTexture;

	return ret;
}

//注: etcpack.exe convert.exe 都需要，否则转换不成功
bool CBlpToKTX::processPvrTexture( pvrtexture::CPVRTexture* pTexture, IBLPImage* blpImage, const char* outputpath )
{
	if(pTexture->getNumMIPLevels() == 0)
	{
		strError = "failed! input texture cannot have mipmaps!";
		_ASSERT(false);
		return false;
	}

	uint32_t width = pTexture->getWidth();
	uint32_t height = pTexture->getHeight();
	bool isCompressed = isCompressedFormat(blpImage->getColorFormat());
	const char* format = getColorFormatString(blpImage->getColorFormat());
	uint8_t alphaDepth = blpImage->getAlphaDepth();
	uint32_t mipLevel = blpImage->getNumMipLevels();

	printf("width: %u, height: %u, format: %s, alpha: %u, mipmap: %u\n", width, height, format, alphaDepth, mipLevel);
	printf("processing...\n");

	ECompressorQuality eQuality = ePVRTCNormal;
	switch (Quality)
	{
	case PVRCQ_Low:
		{
			eQuality = ePVRTCFast;
			break;
		}
	case PVRCQ_Normal:
		{
			eQuality = ePVRTCNormal;
			break;
		}
	case PVRCQ_High:
		{
			eQuality = ePVRTCHigh;
			break;
		}
	case PVRCQ_Best:
		{
			eQuality = ePVRTCBest;
			break;
		}
	default:
		{
			strError = "unsupported quality";
			_ASSERT(false);
			return false;
		}
	}

	if (pTexture->getBitsPerPixel() == 0)
	{
		strError = "failed to create pvr texture!";
		return false;
	}

	if (!isPOT(width) || !isPOT(height))
	{
		strError = "width or height is not power of two";
		return false;
	}

	//convert to a8r8g8b8
	if (blpImage->getColorFormat() != ECF_A8R8G8B8)
	{
		if(!Transcode(*pTexture, PixelType('r','g','b','a',8,8,8,8), ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB, eQuality))
		{
			strError == "cannot convert to A8R8G8B8 format";
			return false;
		}
	}

	//reisze if neccesary
	uint32_t nwidth = width;
	uint32_t nheight = height;

	bool needresize = false;

	if (isCompressed && width != height)		//压缩纹理需要宽高相等
	{
		nwidth = nheight = min_(width, height);
		needresize = true;
	}

	if (width < 4 || height < 4)
	{
		nwidth = nheight = 4;
		needresize = true;
	}

	if (needresize)
	{
		if (!Resize(*pTexture, nwidth, nheight, 1, eResizeLinear))
		{
			strError.format("cannot resize to width: %u, height: %u", nwidth, nheight);
			return false;
		}
	}

	if (isCompressed)
	{
		//generate mipmaps only compressed
// 		if (blpImage->getNumMipLevels() > 1 &&
// 			!GenerateMIPMaps(*pTexture, eResizeLinear))
// 		{
// 			strError = "failed to generate mipmaps";
// 			return false;
// 		}

		//和tga转换一致
		if(!Transcode(*pTexture, PixelType('b','g','r','a',8,8,8,8), ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB, eQuality))
		{
			strError == "cannot convert to A8B8G8R8 format";
			return false;
		}

		//tga中间文件
		if (!TGAWriteFile(g_tempTGAFileName, nwidth, nheight, TGA_FORMAT_BGRA, pTexture->getDataPtr(0)))
		{
			strError = "failed to generate tga intermediate file";
			return false;
		}

		bool hasAlpha = alphaDepth > 0;

		//调用etcpack.exe
		const char* cmd_mipmap   = blpImage->getNumMipLevels() > 1 ? "-mipmaps" : "";
		const char* cmd_quality  = Quality == PVRCQ_Low ? "-s fast" : "-s slow"; 
		const char* cmd_alpha    = hasAlpha ? "-aa" : "";

		//注: etcpack.exe convert.exe 都需要，否则转换不成功
		char cmd[512] = {0,};
		sprintf_s(  cmd,
			"%s %s %s %s %s %s %s %s %s",
			"etcpack.exe", 
			g_tempTGAFileName, 
			"temp", 
			"-ktx", 
			"-c etc1", 
			"-e perceptual",
			cmd_mipmap, 
			cmd_alpha,
			cmd_quality);

		STARTUPINFOA sStartupInfo = { NULL };
		PROCESS_INFORMATION sProcessInfo = { NULL };
		if (FALSE == CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &sStartupInfo, &sProcessInfo))
		{
			strError = "failed to execute etcpack.exe";
			return false;
		}

		WaitForSingleObject(sProcessInfo.hProcess, INFINITE);

		CloseHandle(sProcessInfo.hProcess);
		CloseHandle(sProcessInfo.hThread);

		if (hasAlpha)
			addAlphaMetadata(g_tempKTXFileName);

		DeleteFileA(outputpath);
		if (FALSE == MoveFileA(g_tempKTXFileName, outputpath))
		{
			CSysUtility::outputDebug("cannot move file from %s to %s", g_tempKTXFileName, outputpath);
			_ASSERT(false);
			return false;
		}
		else
		{
			CSysUtility::outputDebug("successfully converted compressed ktx: %s", outputpath);
		}
	} 
	else
	{
		if(!Transcode(*pTexture, PixelType('r','g','b','a',8,8,8,8), ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB, eQuality))
		{
			strError == "cannot convert to A8R8G8B8 format";
			return false;
		}

		CPVRTString strOutputFilePath(outputpath);
		if (!pTexture->saveFile(strOutputFilePath))
		{
			strError.format("failed to save ktx file: %s", outputpath);
			return false;
		}
	}

	printf("successfully output: width: %u, height: %u, mipmap: %u\n", pTexture->getWidth(), pTexture->getHeight(), pTexture->getNumMIPLevels());

	return true;
}

bool CBlpToKTX::addAlphaMetadata( const char* filename )
{
	FILE* file = _fsopen(filename, "rb+", _SH_DENYWR);
	if (!file)
	{
		_ASSERT(false);
		return false;
	}

	fseek(file, 0, SEEK_END);
	uint32_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (size < sizeof(KTX_Header))
	{
		fclose(file);
		_ASSERT(false);
		return false;
	}

	uint8_t* filedata = (uint8_t*)Z_AllocateTempMemory(size);
	if (fread(filedata, 1, size, file) != size)
	{
		fclose(file);
		_ASSERT(false);
		return false;
	}

	KTX_Header* header = reinterpret_cast<KTX_Header*>(filedata);
	uint8_t* textureData = filedata + sizeof(KTX_Header) + header->bytesOfKeyValueData;
	uint32_t textureSize = size - sizeof(KTX_Header) - header->bytesOfKeyValueData;

	//write header
	header->bytesOfKeyValueData = sizeof(KTX_Metadata);
	fseek(file, 0, SEEK_SET);
	if (fwrite(header, 1, sizeof(KTX_Header), file) != sizeof(KTX_Header))
	{
		fclose(file);
		_ASSERT(false);
		return false;
	}

	//write metadata
	KTX_Metadata metadata;
	metadata.byteSize = sizeof(uint32_t) * 2;
	metadata.key = KTX_METADATA_ALPHAINFO;
	metadata.data = 1;
	if (fwrite(&metadata, 1, sizeof(KTX_Metadata), file) != sizeof(KTX_Metadata))
	{
		fclose(file);
		_ASSERT(false);
		return false;
	}

	//write texture data
	if (fwrite(textureData, 1, textureSize, file) != textureSize)
	{
		fclose(file);
		_ASSERT(false);
		return false;
	}

	fclose(file);
	Z_FreeTempMemory(filedata);

	return true;
}

