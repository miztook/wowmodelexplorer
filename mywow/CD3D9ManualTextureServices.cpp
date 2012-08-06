#include "stdafx.h"
#include "CD3D9ManualTextureServices.h"
#include "mywow.h"
#include "CImage.h"
#include "CD3D9Texture.h"

CD3D9ManualTextureServices::CD3D9ManualTextureServices()
{
	loadTextures();
	g_Engine->getDriver()->registerLostReset(this);
}

CD3D9ManualTextureServices::~CD3D9ManualTextureServices()
{
	for (T_TextureMap::iterator itr = TextureMap.begin(); itr != TextureMap.end(); ++itr)
	{
		if ((*itr).second)
			(*itr).second->drop();
	}

	for (T_RTList::iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->drop();
	}
}

void CD3D9ManualTextureServices::loadTextures()
{
	dimension2du size(2,2);
	u32 white[4] =  { 0xFFFFFFFF, 0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF };
	u32 red[4] = { 0xFFFF0000, 0xFFFF0000,0xFFFF0000,0xFFFF0000 };
	u32 blue[4] = { 0xFF0000FF, 0xFF0000FF,0xFF0000FF,0xFF0000FF };
	u32 checker[4] = { 0xFFFFFFFF, 0xFF000000,0xFF000000,0xFFFFFFFF };

	//white
	{
		CImage* w = new CImage(ECF_A8R8G8B8, size, white, false);
		CD3D9Texture* tex = new CD3D9Texture(w, true);
		tex->createVideoTexture();

		_ASSERT(tex->isValid());

		TextureMap["$whiteimage"] = tex;
		w->drop();
	}

	//red
	{
		CImage* w = new CImage(ECF_A8R8G8B8, size, red, false);
		CD3D9Texture* tex = new CD3D9Texture(w, true);
		tex->createVideoTexture();

		_ASSERT(tex->isValid());

		TextureMap["$redimage"] = tex;
		w->drop();
	}

	//blue
	{
		CImage* w = new CImage(ECF_A8R8G8B8, size, blue, false);
		CD3D9Texture* tex = new CD3D9Texture(w, true);
		tex->createVideoTexture();

		_ASSERT(tex->isValid());

		TextureMap["$blueimage"] = tex;
		w->drop();
	}

	//checker
	{
		CImage* w = new CImage(ECF_A8R8G8B8, size, checker, false);
		CD3D9Texture* tex = new CD3D9Texture(w, true);
		tex->createVideoTexture();

		_ASSERT(tex->isValid());

		TextureMap["$checkerimage"] = tex;
		w->drop();
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

ITexture* CD3D9ManualTextureServices::addRenderTarget( dimension2du size, ECOLOR_FORMAT format )
{
	CD3D9Texture* tex = new CD3D9Texture;
	tex->createAsRenderTarget(size, format);
	_ASSERT(tex->isValid());

	RenderTargets.push_back(tex);

	return tex;
}

void CD3D9ManualTextureServices::removeRenderTarget( ITexture* texture )
{
	for (T_RTList::iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		if ((*itr) == texture)
		{
			RenderTargets.erase(itr);
			(*itr)->drop();
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

ITexture* CD3D9ManualTextureServices::createEmptyTexture( dimension2du size, ECOLOR_FORMAT format, bool mipmap )
{
	CD3D9Texture* tex = new CD3D9Texture;
	if (tex->createEmptyTexture(size, format, mipmap))
	{
		_ASSERT(tex->isValid());
		return tex;
	}
	tex->drop();
	return NULL;
}