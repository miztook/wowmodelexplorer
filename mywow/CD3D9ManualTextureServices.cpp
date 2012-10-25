#include "stdafx.h"
#include "CD3D9ManualTextureServices.h"
#include "mywow.h"
#include "CImage.h"
#include "CD3D9Texture.h"
#include "CD3D9RenderTarget.h"

CD3D9ManualTextureServices::CD3D9ManualTextureServices()
{
	g_Engine->getDriver()->registerLostReset(this);
}

CD3D9ManualTextureServices::~CD3D9ManualTextureServices()
{
	for (T_TextureMap::iterator itr = TextureMap.begin(); itr != TextureMap.end(); ++itr)
	{
		if ((*itr).second)
		{
			(*itr).second->drop();
		}
	}

	for (T_RTList::iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		delete (*itr);
	}
}

ITexture* CD3D9ManualTextureServices::getManualTexture(const c8* name)
{
	T_TextureMap::iterator itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return NULL;

	 return (*itr).second;
}

ITexture* CD3D9ManualTextureServices::addTexture( const c8* name , IImage* img )
{
	if(TextureMap.find(name) != TextureMap.end())
		return NULL;

	CD3D9Texture* tex = new CD3D9Texture(img, true);
	tex->createVideoTexture();
	TextureMap[name] =  tex;

	return tex;
}

void CD3D9ManualTextureServices::removeTexture(const c8* name)
{
	T_TextureMap::iterator itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return;

	if ((*itr).second)
	{
		(*itr).second->drop();
	}
	TextureMap.erase(itr);
}

IRenderTarget* CD3D9ManualTextureServices::addRenderTarget( dimension2du size, ECOLOR_FORMAT format )
{
	CD3D9RenderTarget* tex = new CD3D9RenderTarget(size, format);
	_ASSERT(tex->isValid());

	RenderTargets.push_back(tex);

	return tex;
}

void CD3D9ManualTextureServices::removeRenderTarget( IRenderTarget* texture )
{
	for (T_RTList::iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		if ((*itr) == texture)
		{
			RenderTargets.erase(itr);
			delete (*itr);
			break;
		}
	}
}

void CD3D9ManualTextureServices::onLost()
{
	for (T_RTList::iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onLost();
	}
}

void CD3D9ManualTextureServices::onReset()
{
	for (T_RTList::iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onReset();
	}
}

ITexture* CD3D9ManualTextureServices::createTextureFromData( dimension2du size, ECOLOR_FORMAT format, void* data, bool mipmap )
{
	CImage* image = new CImage(format, size, data, false);
	CD3D9Texture* tex = new CD3D9Texture(image, mipmap);
	if (!tex->createVideoTexture())
		tex->drop();
	image->drop();
	return tex;
}

ITexture* CD3D9ManualTextureServices::createTextureFromImage( IImage* image, bool mipmap )
{
	CD3D9Texture* tex = new CD3D9Texture(image, mipmap);
	if (tex->createVideoTexture())
	{
		_ASSERT(tex->isValid());
		return tex;
	}
	tex->drop();

	return NULL;
}

ITexture* CD3D9ManualTextureServices::createEmptyTexture( dimension2du size, ECOLOR_FORMAT format )
{
	CD3D9Texture* tex = new CD3D9Texture;
	if (tex->createEmptyTexture(size, format))
	{
		_ASSERT(tex->isValid());
		return tex;
	}
	tex->drop();
	return NULL;
}

