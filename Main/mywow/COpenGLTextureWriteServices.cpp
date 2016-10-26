#include "stdafx.h"
#include "COpenGLTextureWriteServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLTexture.h"
#include "COpenGLHelper.h"
#include "COpenGLMaterialRenderServices.h"

COpenGLTextureWriter::COpenGLTextureWriter( const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap, bool bTempMemory )
	: ITextureWriter(numMipmap)
{
	TempMemory = bTempMemory;
	TextureSize = size;
	ColorFormat = format;

	u32 bpp = getBytesPerPixelFromFormat(format);

	MipData = new SMipData[NumMipmaps];
	for (u32 i=0; i<NumMipmaps; ++i)
	{
		dimension2du mipsize = size.getMipLevelSize(i);

		u32 pitch, bytes;
		getImagePitchAndBytes(ColorFormat, mipsize.Width, mipsize.Height, pitch, bytes);
		
		MipData[i].pitch = pitch;
		MipData[i].data = TempMemory ? (u8*)Z_AllocateTempMemory(bytes) : new u8[bytes];
	}
}

COpenGLTextureWriter::~COpenGLTextureWriter()
{
	for (u32 i=0; i<NumMipmaps; ++i)
	{
		if (TempMemory)
			Z_FreeTempMemory(MipData[i].data);
		else
			delete[] MipData[i].data;
	}
	delete[] MipData;
}

void* COpenGLTextureWriter::lock( u32 level, u32& pitch )
{
	if (level >= NumMipmaps)
		return NULL_PTR;

	pitch = MipData[level].pitch;
	return MipData[level].data;
}

void COpenGLTextureWriter::unlock( u32 level )
{

}

void COpenGLTextureWriter::initEmptyData()
{
	u32 pitch;
	void* dest = lock(0, pitch);
	memset(dest, 0, pitch * TextureSize.Height);
	unlock(0);
}

bool COpenGLTextureWriter::copyToTexture( ITexture* texture, const recti* descRect /*= NULL_PTR*/ )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(driver->getMaterialRenderServices());

	GLenum pixelFormat;
	GLenum pixelType;
	
	GLint internalFormat = COpenGLHelper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);
	
	ASSERT(texture->getType() == ETT_IMAGE && texture->getSampleCount() == 1);

	bool compressed = isCompressedFormat(ColorFormat);

	if (descRect)
	{
		ASSERT(!texture->hasMipMaps());

		GLint left = descRect->UpperLeftCorner.X;
		GLint top = descRect->UpperLeftCorner.Y;
		GLint width = descRect->getWidth();
		GLint height = descRect->getHeight();

		u32 bpp = getBytesPerPixelFromFormat(ColorFormat);
		const u8* start = MipData[0].data + MipData[0].pitch * top + bpp * left;
		u8* data = (u8*)Z_AllocateTempMemory(bpp * width * height);

		u8* dst = data;
		u8* src = (u8*)start;
		for (s32 h=0; h<height; ++h)
		{
			Q_memcpy(dst, bpp * width, src, bpp * width);
			dst += bpp * width;
			src += MipData[0].pitch;
		}

		//begin bind
		services->applySamplerTexture(0, texture);
		services->applyTextureFilter(0, ETF_NONE);
		services->applyTextureWrap(0, ETA_U, ETC_CLAMP);
		services->applyTextureWrap(0, ETA_V, ETC_CLAMP);
		services->applyTextureMipMap(0, texture->hasMipMaps());

		if (!compressed)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);			//必须，否则默认为4字节对齐，r8g8b8纹理不处理会出错
			glTexSubImage2D(GL_TEXTURE_2D, 0, left, top, width, height, pixelFormat, pixelType, data);
			ASSERT_OPENGL_SUCCESS();
		}
		else
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	
			u32 pitch, compressedSize;
			getImagePitchAndBytes(ColorFormat, width, height, pitch, compressedSize);
			ASSERT(width > 0 && height > 0 );
			driver->getGLExtension()->extGlCompressedTexSubImage2D(GL_TEXTURE_2D, 0, left, top, width, height, internalFormat, compressedSize, data);
		}
		
		Z_FreeTempMemory(data);
		
		//end bind
		services->applySamplerTexture(0, NULL_PTR);
	}
	else
	{
		//begin bind
		services->applySamplerTexture(0, texture);
		services->applyTextureFilter(0, ETF_NONE);
		services->applyTextureWrap(0, ETA_U, ETC_CLAMP);
		services->applyTextureWrap(0, ETA_V, ETC_CLAMP);
		services->applyTextureMipMap(0, texture->hasMipMaps());

		for (u32 i=0; i<NumMipmaps; ++i)
		{
			dimension2du size = TextureSize.getMipLevelSize(i);
			if (!compressed)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);			//必须，否则默认为4字节对齐，r8g8b8纹理不处理会出错
				glTexImage2D(GL_TEXTURE_2D, i, internalFormat, size.Width, size.Height, 0, pixelFormat, pixelType, MipData[i].data);
				ASSERT_OPENGL_SUCCESS();
			}
			else
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	
				u32 pitch, compressedSize;
				getImagePitchAndBytes(ColorFormat, size.Width, size.Height, pitch, compressedSize);
				ASSERT(size.Width > 0 && size.Height > 0);
				driver->getGLExtension()->extGlCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, size.Width, size.Height, 0, compressedSize, MipData[i].data);
			}
		}

		//end bind
		services->applySamplerTexture(0, NULL_PTR);
	}

	return true;
}

COpenGLTextureWriteServices::COpenGLTextureWriteServices()
{

}

COpenGLTextureWriteServices::~COpenGLTextureWriteServices()
{
	for (T_TextureWriterMap::iterator itr = TextureWriterMap.begin(); itr != TextureWriterMap.end(); ++itr)
	{
		delete itr->second;
	}
	TextureWriterMap.clear();
}

ITextureWriter* COpenGLTextureWriteServices::createTextureWriter( ITexture* texture, bool temp )
{
	ECOLOR_FORMAT format = texture->getColorFormat();
	dimension2du size = texture->getSize();
	u32 numMipmap = texture->getNumMipmaps();

	T_TextureWriterMap::iterator itr = TextureWriterMap.find(texture);
	if (itr != TextureWriterMap.end())
	{
		return itr->second;
	}

	//not found, create
	COpenGLTextureWriter* writer = new COpenGLTextureWriter(size, format, numMipmap, temp);
	TextureWriterMap[texture] = writer;

	return writer;
}

bool COpenGLTextureWriteServices::removeTextureWriter( ITexture* texture )
{
	T_TextureWriterMap::iterator itr = TextureWriterMap.find(texture);
	if (itr != TextureWriterMap.end())
	{
		delete itr->second;
		TextureWriterMap.erase(itr);
		return true;
	}

	return false;
}



#endif


