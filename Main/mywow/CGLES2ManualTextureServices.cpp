#include "stdafx.h"
#include "CGLES2ManualTextureServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CImage.h"
#include "CGLES2Texture.h"
#include "CGLES2RenderTarget.h"
#include "CGLES2Driver.h"

CGLES2ManualTextureServices::CGLES2ManualTextureServices()
{
	Driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	Driver->registerLostReset(this);

	loadDefaultTextures();
}

CGLES2ManualTextureServices::~CGLES2ManualTextureServices()
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

ITexture* CGLES2ManualTextureServices::addTexture( const c8* name, const dimension2du& size, IImage* img, bool mipmap )
{
	if(TextureMap.find(name) != TextureMap.end())
		return NULL;

	CGLES2Texture* tex = new CGLES2Texture(mipmap);
	if(!tex->createFromImage(size, img))
	{
		tex->drop();
		return NULL;
	}
	TextureMap[name] =  tex;

	return tex;
}

void CGLES2ManualTextureServices::removeTexture( const c8* name )
{
	T_TextureMap::iterator itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return;

	if ((*itr).second)
		(*itr).second->drop();

	TextureMap.erase(itr);
}

IRenderTarget* CGLES2ManualTextureServices::addRenderTarget( const dimension2du& size, ECOLOR_FORMAT format )
{
	CGLES2RenderTarget* tex = new CGLES2RenderTarget(size, format);
	ASSERT(tex->isValid());

	RenderTargets.push_back(tex);

	return tex;
}

void CGLES2ManualTextureServices::removeRenderTarget( IRenderTarget* texture )
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

ITexture* CGLES2ManualTextureServices::createTextureFromImage( const dimension2du& size, IImage* image, bool mipmap )
{
	CGLES2Texture* tex = new CGLES2Texture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL;
	}
	return tex;
}

ITexture* CGLES2ManualTextureServices::createTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	CImage* image = new CImage(format, size, data, false);
	CGLES2Texture* tex = new CGLES2Texture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL;
	}
	image->drop();
	return tex;
}

ITexture* CGLES2ManualTextureServices::createCompressTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	ASSERT(false);
	return NULL;
}

ITexture* CGLES2ManualTextureServices::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	CGLES2Texture* tex = new CGLES2Texture(false);
	if (tex->createEmptyTexture(size, format))
	{
		ASSERT(tex->isValid());
		return tex;
	}
	tex->drop();
	return NULL;
}

void CGLES2ManualTextureServices::onLost()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onLost();
	}
}

void CGLES2ManualTextureServices::onReset()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onReset();
	}
}

void CGLES2ManualTextureServices::loadDefaultTextures()
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