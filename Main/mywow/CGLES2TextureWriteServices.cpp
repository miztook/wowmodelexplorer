#include "stdafx.h"
#include "CGLES2TextureWriteServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"
#include "CGLES2Texture.h"
#include "CGLES2Helper.h"
#include "CGLES2MaterialRenderServices.h"

CGLES2TextureWriter::CGLES2TextureWriter( const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap, bool bTempMemory )
	: ITextureWriter(numMipmap)
{
	Driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());

	TextureSize = size;
	ColorFormat = format;

	MipData = new SMipData[NumMipmaps];
	for (u32 i=0; i<NumMipmaps; ++i)
	{
		dimension2du mipsize = size.getMipLevelSize(i);

		u32 pitch, bytes;
		getImagePitchAndBytes(ColorFormat, mipsize.Width, mipsize.Height, pitch, bytes);

		MipData[i].pitch = pitch;
		MipData[i].data = new u8[bytes];
	}
}

CGLES2TextureWriter::~CGLES2TextureWriter()
{
	for (u32 i=0; i<NumMipmaps; ++i)
	{
		delete[] MipData[i].data;
	}
	delete[] MipData;
}

void* CGLES2TextureWriter::lock( u32 level, u32& pitch )
{
	if (level >= NumMipmaps)
		return NULL;

	pitch = MipData[level].pitch;
	return MipData[level].data;
}

void CGLES2TextureWriter::unlock( u32 level )
{

}

void CGLES2TextureWriter::initEmptyData()
{
	u32 pitch;
	void* dest = lock(0, pitch);
	memset(dest, 0, pitch * TextureSize.Height);
	unlock(0);
}

bool CGLES2TextureWriter::copyToTexture( ITexture* texture, const recti* descRect /*= NULL*/ )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	ASSERT(!texture->isLockable());

	GLenum pixelFormat;
	GLenum pixelType;

	GLint internalFormat = CGLES2Helper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

	ASSERT(texture->getType() == ETT_IMAGE);

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
			ASSERT_GLES2_SUCCESS();
		}
		else
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	
			u32 pitch, compressedSize;
			getImagePitchAndBytes(ColorFormat, width, height, pitch, compressedSize);
			ASSERT(width > 0 && height > 0);
			Driver->getGLExtension()->extGlCompressedTexSubImage2D(GL_TEXTURE_2D, 0, left, top, width, height, internalFormat, compressedSize, data);
		}
		
		Z_FreeTempMemory(data);
		
		//end bind
		services->applySamplerTexture(0, NULL);
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
				ASSERT_GLES2_SUCCESS();
			}
			else
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	
				u32 pitch, compressedSize;
				getImagePitchAndBytes(ColorFormat, size.Width, size.Height, pitch, compressedSize);
				ASSERT(size.Width > 0 && size.Height > 0);
				Driver->getGLExtension()->extGlCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, size.Width, size.Height, 0, compressedSize, MipData[i].data);
			}
		}

		//end bind
		services->applySamplerTexture(0, NULL);
	}

	return true;
}

CGLES2TextureWriteServices::CGLES2TextureWriteServices()
{

}

CGLES2TextureWriteServices::~CGLES2TextureWriteServices()
{
	for (T_TextureWriterMap::iterator itr = TextureWriterMap.begin(); itr != TextureWriterMap.end(); ++itr)
	{
		delete itr->second;
	}
	TextureWriterMap.clear();
}

ITextureWriter* CGLES2TextureWriteServices::createTextureWriter( ITexture* texture, bool temp )
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
	CGLES2TextureWriter* writer = new CGLES2TextureWriter(size, format, numMipmap, temp);
	TextureWriterMap[texture] = writer;

	return writer;
}

bool CGLES2TextureWriteServices::removeTextureWriter( ITexture* texture )
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