#include "stdafx.h"
#include "CD3D9ManualTextureServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CImage.h"
#include "CBLPImage.h"
#include "CD3D9Texture.h"
#include "CD3D9RenderTarget.h"
#include "CD3D9Driver.h"

CD3D9ManualTextureServices::CD3D9ManualTextureServices()
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	Driver->registerLostReset(this);

	loadDefaultTextures();
}

CD3D9ManualTextureServices::~CD3D9ManualTextureServices()
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

ITexture* CD3D9ManualTextureServices::addTexture( const c8* name, const dimension2du& size, IImage* img, bool mipmap )
{
	if(TextureMap.find(name) != TextureMap.end())
		return NULL_PTR;

	CD3D9Texture* tex = new CD3D9Texture(mipmap);
	if(!tex->createFromImage(size, img))
	{
		tex->drop();
		return NULL_PTR;
	}
	TextureMap[name] =  tex;

	return tex;
}

void CD3D9ManualTextureServices::removeTexture(const c8* name)
{
	T_TextureMap::iterator itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return;

	if ((*itr).second)
		(*itr).second->drop();

	TextureMap.erase(itr);
}

IRenderTarget* CD3D9ManualTextureServices::addRenderTarget( const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt )
{
	CD3D9RenderTarget* tex = new CD3D9RenderTarget(size, colorFmt, depthFmt);
	ASSERT(tex->isValid());

	RenderTargets.push_back(tex);

	return tex;
}

void CD3D9ManualTextureServices::removeRenderTarget( IRenderTarget* texture )
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

void CD3D9ManualTextureServices::onLost()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onLost();
	}
}

void CD3D9ManualTextureServices::onReset()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onReset();
	}
}

ITexture* CD3D9ManualTextureServices::createTextureFromImage( const dimension2du& size, IImage* image, bool mipmap )
{
	CD3D9Texture* tex = new CD3D9Texture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	return tex;
}

ITexture* CD3D9ManualTextureServices::createTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	CImage* image = new CImage(format, size, data, false);
	CD3D9Texture* tex = new CD3D9Texture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	image->drop();
	return tex;
}

ITexture* CD3D9ManualTextureServices::createCompressTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	CBLPImage* image = new CBLPImage;
	if (!image->fromImageData((const u8*)data, size, format, mipmap))
	{
		image->drop();
		return NULL_PTR;
	}

	CD3D9Texture* tex = new CD3D9Texture(mipmap);
	if (!tex->createFromBlpImage(image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	image->drop();
	return tex;
}

ITexture* CD3D9ManualTextureServices::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	CD3D9Texture* tex = new CD3D9Texture(false);
	if (tex->createEmptyTexture(size, format))
	{
		ASSERT(tex->isValid());
		return tex;
	}
	tex->drop();
	return NULL_PTR;
}

void CD3D9ManualTextureServices::loadDefaultTextures()
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