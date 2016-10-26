#include "stdafx.h"
#include "COpenGLTexture.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLHelper.h"
#include "CLock.h"
#include "CImage.h"
#include "COpenGLMaterialRenderServices.h"
#include "COpenGLExtension.h"
#include "CBlit.h"

COpenGLTexture::COpenGLTexture( bool mipmap )
	: GLTexture(0)
{
	HasMipMaps = mipmap;
}

COpenGLTexture::~COpenGLTexture()
{
	
}

bool COpenGLTexture::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	HasMipMaps = false;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLTexture::createEmptyTexture Failed");
		ASSERT(false);
		return false;
	}

	if (!HasMipMaps)
	{
		ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);
		texWriter->initEmptyData();
		texWriter->copyToTexture(this);
		g_Engine->getTextureWriteServices()->removeTextureWriter(this);
	}

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::createFromImage( const dimension2du& size, IImage* image )
{
	//CLock lock(&g_Globals.textureCS);

	if(VideoBuilt || !image || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? image->getDimension().getNumMipLevels() : 1;
	TextureSize = size;	//image->getDimension();
	ColorFormat = image->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLTexture::createFromImage Failed");
		ASSERT(false);
		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, image);

	if (HasMipMaps)
		createMipMaps(texWriter);				//填充mipmap

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::createFromBlpImage( IBLPImage* blpImage )
{
	//CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	if(!blpImage || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? blpImage->getNumMipLevels() : 1;
	TextureSize = blpImage->getDimension();
	ColorFormat = blpImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLTexture::createFromBlpImage Failed");
		ASSERT(false);

		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, blpImage);

	if (HasMipMaps)
		copyBlpMipMaps(texWriter, blpImage);

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::createRTTexture( const dimension2du& size, ECOLOR_FORMAT format, int nSamples )
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	Type = ETT_RENDERTARGET;
	HasMipMaps = false;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;
	SampleCount = nSamples;

	glGenTextures(1, &GLTexture);
	ASSERT_OPENGL_SUCCESS();

	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	GLenum pixelFormat;
	GLenum pixelType;

	GLint internalFormat = COpenGLHelper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

	if (SampleCount == 0)
	{
		//begin bind
		services->applySamplerTexture(0, this);
		services->applyTextureFilter(0, ETF_NONE);
		services->applyTextureWrap(0, ETA_U, ETC_CLAMP);
		services->applyTextureWrap(0, ETA_V, ETC_CLAMP);
		services->applyTextureMipMap(0, false);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.Width, size.Height, 0, pixelFormat, pixelType, NULL_PTR);
		ASSERT_OPENGL_SUCCESS();

		//end bind
		services->applySamplerTexture(0, NULL_PTR);
	}
	else
	{
		services->applySamplerTextureMultiSample(0, this);

		COpenGLExtension* ext = static_cast<COpenGLDriver*>(g_Engine->getDriver())->getGLExtension();
		ext->extGlTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleCount, internalFormat, size.Width, size.Height,  GL_TRUE);

		services->applySamplerTextureMultiSample(0, 0);
	}

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::createVideoTexture()
{
	//CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	IBLPImage* blpImage = g_Engine->getResourceLoader()->loadBLP(getFileName());

	if(!blpImage || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? blpImage->getNumMipLevels() : 1;
	TextureSize = blpImage->getDimension();
	ColorFormat = blpImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLTexture::createVideoTexture Failed");

		blpImage->drop();

		ASSERT(false);
		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, blpImage);

	if (HasMipMaps)
		copyBlpMipMaps(texWriter, blpImage);

	blpImage->drop();

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

void COpenGLTexture::releaseVideoTexture()
{
	if (VideoBuilt)
	{
		//CLock lock(&g_Globals.textureCS);
		if (GLTexture)
		{
			glDeleteTextures(1, &GLTexture);
			GLTexture = 0;
		}
		VideoBuilt = false;
	}
}

void COpenGLTexture::copyTexture( ITextureWriter* writer, IBLPImage* blpimage )
{
	ASSERT(blpimage);

	u32 pitch;
	void* destData = writer->lock(0, pitch);

	bool result = blpimage->copyMipmapData(0, destData, pitch, TextureSize.Width, TextureSize.Height);
	ASSERT(result);

	writer->unlock(0);
}

void COpenGLTexture::copyTexture( ITextureWriter* writer, IImage* image )
{
	ASSERT(image);

	u32 pitch;
	void* destData = writer->lock(0, pitch);

	image->copyToScaling(destData, TextureSize.Width, TextureSize.Height, ColorFormat, pitch);

	writer->unlock(0);
}

bool COpenGLTexture::copyBlpMipMaps( ITextureWriter* writer, IBLPImage* blpimage, u32 level /*= 1*/ )
{
	if (!HasMipMaps || !blpimage)
		return false;

	if (level==0 || level >=NumMipmaps)
		return true;

	dimension2du size = TextureSize.getMipLevelSize(level);

	//lock
	u32 pitch;
	void* destData = writer->lock(level, pitch);

	bool result = blpimage->copyMipmapData(level, destData, pitch, size.Width, size.Height);

	// unlock
	writer->unlock(level);

	if (!result || (size.Width < 2 && size.Height < 2))
		return false; // stop generating levels

	// generate next level
	return copyBlpMipMaps(writer, blpimage, level+1);
}

bool COpenGLTexture::createMipMaps( ITextureWriter* writer, u32 level /*= 1 */ )
{
	ASSERT(HasMipMaps);

	if (level==0 || level >= NumMipmaps)
		return true;

	dimension2du upperSize = TextureSize.getMipLevelSize(level - 1);
	dimension2du lowerSize = TextureSize.getMipLevelSize(level);

	// lock upper surface
	u32 upperPitch, lowerPitch;
	const u8* upperDest = (const u8*)writer->lock(level - 1, upperPitch);
	u8*	lowerDest = (u8*)writer->lock(level, lowerPitch);

if (ColorFormat == ECF_R5G6B5 || ColorFormat == ECF_A1R5G5B5)
		CBlit::copy16BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch, ColorFormat);
	else if (ColorFormat == ECF_A8R8G8B8)
		CBlit::copy32BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch, ColorFormat);
	else if (ColorFormat == ECF_R8G8B8)
		CBlit::copyR8G8B8BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch);
	else
		ASSERT(false);				//不支持的格式

	writer->unlock(level);
	writer->unlock(level - 1);

	if (lowerSize.Width < 2 && lowerSize.Height < 2)
		return false; // stop generating levels

	// generate next level
	return createMipMaps(writer, level+1);
}

bool COpenGLTexture::createTexture( const dimension2du& size, ECOLOR_FORMAT format, bool mipmap /*= true */ )
{
	if(VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	glGenTextures(1, &GLTexture);
	ASSERT_OPENGL_SUCCESS();

	if (!mipmap)
	{
		COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

		GLenum pixelFormat;
		GLenum pixelType;

		GLint internalFormat = COpenGLHelper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

		//begin bind
		services->applySamplerTexture(0, this);
		services->applyTextureFilter(0, ETF_NONE);
		services->applyTextureWrap(0, ETA_U, ETC_CLAMP);
		services->applyTextureWrap(0, ETA_V, ETC_CLAMP);
		services->applyTextureMipMap(0, false);

		//end bind
		services->applySamplerTexture(0, NULL_PTR);
	}
	
	return true;
}


#endif