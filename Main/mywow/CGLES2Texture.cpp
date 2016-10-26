#include "stdafx.h"
#include "CGLES2Texture.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Helper.h"
#include "CImage.h"
#include "CGLES2MaterialRenderServices.h"
#include "CBlit.h"

CGLES2Texture::CGLES2Texture( bool mipmap )
	: GLTexture(0)
{
	HasMipMaps = mipmap;
}

CGLES2Texture::~CGLES2Texture()
{

}

bool CGLES2Texture::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	CLock lock(&g_Globals.textureCS);

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
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2Texture::createEmptyTexture Failed");
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

bool CGLES2Texture::createFromImage( const dimension2du& size, IImage* image )
{
	CLock lock(&g_Globals.textureCS);

	if(VideoBuilt || !image || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? image->getDimension().getNumMipLevels() : 1;
	TextureSize = size; //image->getDimension();
	ColorFormat = image->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2Texture::createFromImage Failed");
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

bool CGLES2Texture::createFromBlpImage( IBLPImage* blpImage )
{
	CLock lock(&g_Globals.textureCS);
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
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2Texture::createFromBlpImage Failed");
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

bool CGLES2Texture::createFromPvrImage( IPVRImage* pvrImage )
{
	CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	if(!pvrImage || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? pvrImage->getNumMipLevels() : 1;
	TextureSize = pvrImage->getDimension();
	ColorFormat = pvrImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2Texture::createFromPvrImage Failed");
		ASSERT(false);

		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, pvrImage);

	if (HasMipMaps)
		copyPvrMipMaps(texWriter, pvrImage);

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

bool CGLES2Texture::createFromKtxImage( IKTXImage* ktxImage )
{
	CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	if(!ktxImage || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? ktxImage->getNumMipLevels() : 1;
	TextureSize = ktxImage->getDimension();
	ColorFormat = ktxImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2Texture::createFromKtxImage Failed");
		ASSERT(false);

		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, ktxImage);

	if (HasMipMaps)
		copyKtxMipMaps(texWriter, ktxImage);

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

bool CGLES2Texture::createRTTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	CLock lock(&g_Globals.textureCS);

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

	glGenTextures(1, &GLTexture);
	ASSERT_GLES2_SUCCESS();

	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	GLenum pixelFormat;
	GLenum pixelType;

	GLint internalFormat = CGLES2Helper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

	//begin bind
	services->applySamplerTexture(0, this);
	services->applyTextureFilter(0, ETF_BILINEAR);
	services->applyTextureWrap(0, ETA_U, ETC_CLAMP);
	services->applyTextureWrap(0, ETA_V, ETC_CLAMP);
	services->applyTextureMipMap(0, false);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.Width, size.Height, 0, pixelFormat, pixelType, 0);
	ASSERT_GLES2_SUCCESS();

	//end bind
	services->applySamplerTexture(0, NULL);

	VideoBuilt = true;

	return true;
}

bool CGLES2Texture::createVideoTexture()
{
	CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

#if defined(USE_PVR)

	IPVRImage* pvrImage = g_Engine->getResourceLoader()->loadPVR(getFileName());

	if(!pvrImage || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? pvrImage->getNumMipLevels() : 1;
	TextureSize = pvrImage->getDimension();
	ColorFormat = pvrImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLTexture::createVideoTexture Failed");

		pvrImage->drop();

		ASSERT(false);
		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, pvrImage);

	if (HasMipMaps)
		copyPvrMipMaps(texWriter, pvrImage);

	pvrImage->drop();

#elif defined(USE_KTX)

	IKTXImage* ktxImage = g_Engine->getResourceLoader()->loadKTX(getFileName());

	if(!ktxImage || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? ktxImage->getNumMipLevels() : 1;
	TextureSize = ktxImage->getDimension();
	ColorFormat = ktxImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLTexture::createVideoTexture Failed");

		ktxImage->drop();

		ASSERT(false);
		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, ktxImage);

	if (HasMipMaps)
		copyKtxMipMaps(texWriter, ktxImage);

	ktxImage->drop();
#endif

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

void CGLES2Texture::releaseVideoTexture()
{
	if (VideoBuilt)
	{
		CLock lock(&g_Globals.textureCS);
		if (GLTexture)
		{
			glDeleteTextures(1, &GLTexture);
			GLTexture = 0;
		}
		VideoBuilt = false;
	}
}

void CGLES2Texture::copyTexture( ITextureWriter* writer, IImage* image )
{
	ASSERT(image);

	u32 pitch;
	void* destData = writer->lock(0, pitch);

	image->copyToScaling(destData, TextureSize.Width, TextureSize.Height, ColorFormat, pitch);

	writer->unlock(0);
}

void CGLES2Texture::copyTexture( ITextureWriter* writer, IBLPImage* blpimage )
{
	ASSERT(blpimage);

	u32 pitch;
	void* destData = writer->lock(0, pitch);

	bool result = blpimage->copyMipmapData(0, destData, pitch, TextureSize.Width, TextureSize.Height);
	ASSERT(result);

	writer->unlock(0);
}

void CGLES2Texture::copyTexture( ITextureWriter* writer, IPVRImage* pvrimage )
{
	ASSERT(pvrimage);

	u32 pitch;
	void* destData = writer->lock(0, pitch);

	bool result = pvrimage->copyMipmapData(0, destData, pitch, TextureSize.Width, TextureSize.Height);
	ASSERT(result);

	writer->unlock(0);
}

void CGLES2Texture::copyTexture( ITextureWriter* writer, IKTXImage* ktxImage )
{
	ASSERT(ktxImage);

	u32 pitch;
	void* destData = writer->lock(0, pitch);

	bool result = ktxImage->copyMipmapData(0, destData, pitch, TextureSize.Width, TextureSize.Height);
	ASSERT(result);

	writer->unlock(0);
}

bool CGLES2Texture::copyBlpMipMaps( ITextureWriter* writer, IBLPImage* blpimage, u32 level /*= 1*/ )
{
	if (!HasMipMaps || !blpimage)
		return false;

	if (level==0 || level >= NumMipmaps)
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

bool CGLES2Texture::copyPvrMipMaps( ITextureWriter* writer, IPVRImage* pvrimage, u32 level /*= 1*/ )
{
	if (!HasMipMaps || !pvrimage)
		return false;

	if (level==0 || level >= NumMipmaps)
		return true;

	dimension2du size = TextureSize.getMipLevelSize(level);

	//lock
	u32 pitch;
	void* destData = writer->lock(level, pitch);

	bool result = pvrimage->copyMipmapData(level, destData, pitch, size.Width, size.Height);

	// unlock
	writer->unlock(level);

	if (!result || (size.Width < 2 && size.Height < 2))
		return false; // stop generating levels

	// generate next level
	return copyPvrMipMaps(writer, pvrimage, level+1);
}

bool CGLES2Texture::copyKtxMipMaps( ITextureWriter* writer, IKTXImage* ktxImage, u32 level /*= 1*/ )
{
	if (!HasMipMaps || !ktxImage)
		return false;

	if (level==0 || level >= NumMipmaps)
		return true;

	dimension2du size = TextureSize.getMipLevelSize(level);

	//lock
	u32 pitch;
	void* destData = writer->lock(level, pitch);

	bool result = ktxImage->copyMipmapData(level, destData, pitch, size.Width, size.Height);

	// unlock
	writer->unlock(level);

	if (!result || (size.Width < 2 && size.Height < 2))
		return false; // stop generating levels

	// generate next level
	return copyKtxMipMaps(writer, ktxImage, level+1);
}

bool CGLES2Texture::createMipMaps( ITextureWriter* writer, u32 level /*= 1 */ )
{
	ASSERT(HasMipMaps);

	if (level==0 || level >= NumMipmaps)
		return true;

	dimension2du lowerSize = TextureSize.getMipLevelSize(level);

	// lock upper surface
	u32 upperPitch, lowerPitch;
	const u8* upperDest = (const u8*)writer->lock(level - 1, upperPitch);
	u8*	lowerDest = (u8*)writer->lock(level, lowerPitch);

	if (ColorFormat == ECF_R5G6B5 || ColorFormat == ECF_A1R5G5B5)
		CBlit::copy16BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch, ColorFormat);
	else if (ColorFormat == ECF_A8B8G8R8 || ColorFormat == ECF_A8R8G8B8)
		CBlit::copy32BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch, ColorFormat);
	else if (ColorFormat == ECF_R8G8B8)
		CBlit::copyR8G8B8BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch);
	else
		ASSERT(false);				//不支持的格式

	bool result=true;
	// unlock

	writer->unlock(level);
	writer->unlock(level - 1);

	if (!result || (lowerSize.Width < 2 && lowerSize.Height < 2))
		return false; // stop generating levels

	// generate next level
	return createMipMaps(writer, level+1);
}

bool CGLES2Texture::createTexture( const dimension2du& size, ECOLOR_FORMAT format, bool mipmap /*= true */ )
{
	if(VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	glGenTextures(1, &GLTexture);
	ASSERT_GLES2_SUCCESS();

	if (!mipmap)
	{
		CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

		GLenum pixelFormat;
		GLenum pixelType;

		GLint internalFormat = CGLES2Helper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

		//begin bind
		services->applySamplerTexture(0, this);
		services->applyTextureFilter(0, ETF_NONE);
		services->applyTextureWrap(0, ETA_U, ETC_CLAMP);
		services->applyTextureWrap(0, ETA_V, ETC_CLAMP);
		services->applyTextureMipMap(0, false);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.Width, size.Height, 0, pixelFormat, pixelType, 0);
		ASSERT_GLES2_SUCCESS();

		//end bind
		services->applySamplerTexture(0, NULL);
	}

	return true;
}

#endif