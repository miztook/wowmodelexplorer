#include "stdafx.h"
#include "BlpToPVR.h"
#include "mywow.h"
#include "PVRTextureHeader.h"
#include "PVRTexture.h"
#include "PVRTextureUtilities.h"

using namespace pvrtexture;

CBlpToPVR::CBlpToPVR(PVRCompressionQuality quality) : Quality(quality)
{

}

bool CBlpToPVR::convertBlpToPVR( IBLPImage* blp, const c8* outputpath )
{
	if (!hasFileExtensionA(outputpath, "pvr"))
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

	u32 mipLevels = blp->getNumMipLevels();
	bool compressed =  isCompressedFormat(format);
	u32 bpp = getBytesPerPixelFromFormat(format);

	CPVRTextureHeader PVRHeader(nPVRPixelType, size.Height, size.Width, 1, mipLevels, 1, 1);
	CPVRTexture* pTexture = NULL;
	
	u32 mipDataSize[16] = {0};
	u32 mipDataPitch[16] = {0};
	_ASSERT(mipLevels <= 16);

	u32 dataSize = 0;
	for (u32 i=0; i<mipLevels; ++i)
	{
		dimension2du mipsize = size.getMipLevelSize(i);
		getImagePitchAndBytes(format, mipsize.Width, mipsize.Height, mipDataPitch[i], mipDataSize[i]);
		dataSize += mipDataSize[i];
	}
	
	u32 offset = 0;
	u8* data = (u8*)Z_AllocateTempMemory(dataSize);
	for (u32 i=0; i<mipLevels; ++i)
	{
		dimension2du mipsize = size.getMipLevelSize(i);
		blp->copyMipmapData(i, data + offset, mipDataPitch[i], mipsize.Width, mipsize.Height);

		offset += mipDataSize[i];
	}
	_ASSERT(offset == dataSize);

	pTexture = new CPVRTexture(PVRHeader, data);

	Z_FreeTempMemory(data);

	bool ret = processPvrTexture(pTexture, blp, outputpath, false);

	delete pTexture;

	return ret;
}

bool CBlpToPVR::processPvrTexture( CPVRTexture* pTexture, IBLPImage* blpImage, const c8* outputpath, bool forceUnCompress )
{	
	u32 width = pTexture->getWidth();
	u32 height = pTexture->getHeight();
	bool isCompressed = isCompressedFormat(blpImage->getColorFormat());
	const c8* format = getColorFormatString(blpImage->getColorFormat());
	u8 alphaDepth = blpImage->getAlphaDepth();
	u32 mipLevel = blpImage->getNumMipLevels();

	printf("width: %u, height: %u, format: %s, alpha: %u, mipmap: %u\n", width, height, format, alphaDepth, mipLevel);
	printf("processing...\n");

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
	if (blpImage->getColorFormat() != ECF_A8B8G8R8)
	{
		if(!Transcode(*pTexture, PixelType('r','g','b','a',8,8,8,8), ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB))
		{
			strError == "cannot convert to A8R8G8B8 format";
			return false;
		}
	}

	//reisze if neccesary
	u32 nwidth = width;
	u32 nheight = height;

	bool needresize = false;

	if (isCompressed && width != height)			//压缩纹理需要宽高相等
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

		if (blpImage->getNumMipLevels() > 1 &&
			!GenerateMIPMaps(*pTexture, eResizeLinear))
		{
			strError = "failed to generate mipmaps";
			return false;
		}
	}

	//convert to pvrtc
	PixelType outputPixelType = (isCompressed && !forceUnCompress) ?
		(alphaDepth > 0 ? PixelType(ePVRTPF_PVRTCI_4bpp_RGBA) : PixelType(ePVRTPF_PVRTCI_4bpp_RGB)) :
		//(alphaDepth > 0 ? PixelType('r','g','b','a',8,8,8,8) : PixelType('r','g','b',0,8,8,8,0));
		PixelType('r','g','b','a',8,8,8,8);

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

	if (!Transcode(*pTexture, outputPixelType, ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB, eQuality))
	{
		strError.format("failed to transcode to format %s", getPvrPixelFormatString(outputPixelType));
		return false;
	}

	
	CPVRTString strOutputFilePath(outputpath);
//	if (!pTexture->saveFileLegacyPVR(strOutputFilePath, eOGLES2))
	if (!pTexture->saveFile(strOutputFilePath))
	{
		strError.format("failed to save pvr file: %s", outputpath);
		return false;
	}
	
	printf("successfully output: width: %u, height: %u, mipmap: %u\n", pTexture->getWidth(), pTexture->getHeight(), pTexture->getNumMIPLevels());

	return true;
}

c8* CBlpToPVR::getPvrPixelFormatString( pvrtexture::PixelType type )
{
	if (type.PixelTypeID == PixelType(ePVRTPF_PVRTCI_4bpp_RGBA).PixelTypeID)
	{
		return "PVRTCI_4bpp_RGBA";
	}
	else if (type.PixelTypeID == PixelType(ePVRTPF_PVRTCI_4bpp_RGB).PixelTypeID)
	{
		return "PVRTCI_4bpp_RGB";
	}
	else if (type.PixelTypeID == PixelType('r','g','b','a',8,8,8,8).PixelTypeID)
	{
		return "RGBA8888";
	}
	else if (type.PixelTypeID == PixelType('r','g','b',0,8,8,8,0).PixelTypeID)
	{
		return "RGB888";
	}
	else
	{
		return "unknown";
	}
}