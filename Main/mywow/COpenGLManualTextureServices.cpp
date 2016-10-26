#include "stdafx.h"
#include "COpenGLManualTextureServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "CImage.h"
#include "CBLPImage.h"
#include "COpenGLTexture.h"
#include "COpenGLRenderTarget.h"
#include "COpenGLDriver.h"

COpenGLManualTextureServices::COpenGLManualTextureServices()
{
	Driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	Driver->registerLostReset(this);

	loadDefaultTextures();
}

COpenGLManualTextureServices::~COpenGLManualTextureServices()
{	
	Driver->removeLostReset(this);

	for (T_TextureMap::const_iterator itr = TextureMap.begin(); itr != TextureMap.end(); ++itr)
	{
		ITexture* tex = itr->second;
		if (tex)
			tex->drop();
	}

	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		delete (*itr);
	}
}

ITexture* COpenGLManualTextureServices::addTexture( const c8* name, const dimension2du& size, IImage* img, bool mipmap )
{
	if(TextureMap.find(name) != TextureMap.end())
		return NULL_PTR;

	COpenGLTexture* tex = new COpenGLTexture(mipmap);
	if(!tex->createFromImage(size, img))
	{
		tex->drop();
		return NULL_PTR;
	}
	TextureMap[name] =  tex;

	return tex;
}

void COpenGLManualTextureServices::removeTexture( const c8* name )
{
	T_TextureMap::iterator itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return;

	if ((*itr).second)
		(*itr).second->drop();

	TextureMap.erase(itr);
}

IRenderTarget* COpenGLManualTextureServices::addRenderTarget( const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt )
{
	COpenGLRenderTarget* tex = new COpenGLRenderTarget(size, colorFmt, depthFmt);
	ASSERT(tex->isValid());

	RenderTargets.push_back(tex);

	return tex;
}

void COpenGLManualTextureServices::removeRenderTarget( IRenderTarget* texture )
{
	for (T_RTList::iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		if ((*itr) == texture)
		{
			delete (*itr);
			RenderTargets.erase(itr);
			break;
		}
	}
}

ITexture* COpenGLManualTextureServices::createTextureFromImage( const dimension2du& size, IImage* image, bool mipmap )
{
	COpenGLTexture* tex = new COpenGLTexture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	return tex;
}

ITexture* COpenGLManualTextureServices::createTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	CImage* image = new CImage(format, size, data, false);
	COpenGLTexture* tex = new COpenGLTexture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	image->drop();
	return tex;
}

ITexture* COpenGLManualTextureServices::createCompressTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	CBLPImage* image = new CBLPImage;
	if (!image->fromImageData((const u8*)data, size, format, mipmap))
	{
		image->drop();
		return NULL_PTR;
	}

	COpenGLTexture* tex = new COpenGLTexture(mipmap);
	if (!tex->createFromBlpImage(image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	image->drop();
	return tex;
}

ITexture* COpenGLManualTextureServices::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	COpenGLTexture* tex = new COpenGLTexture(false);
	if (tex->createEmptyTexture(size, format))
	{
		ASSERT(tex->isValid());
		return tex;
	}
	tex->drop();
	return NULL_PTR;
}

void COpenGLManualTextureServices::onLost()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onLost();
	}
}

void COpenGLManualTextureServices::onReset()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onReset();
	}
}

void COpenGLManualTextureServices::loadDefaultTextures()
{
	ECOLOR_FORMAT format = ECF_R5G6B5;
	dimension2du size = dimension2du(32, 32);

	u32 pitch, nbytes;
	getImagePitchAndBytes(format, size.Width, size.Height, pitch, nbytes);

	void* data = Z_AllocateTempMemory(nbytes);
	memset(data, 0xff, nbytes);

	CImage* img = new CImage(format, size, data, false);
	addTexture("$DefaultWhite", size, img, true);
	img->drop();

	Z_FreeTempMemory(data);
}


#endif


