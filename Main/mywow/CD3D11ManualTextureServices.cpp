#include "stdafx.h"
#include "CD3D11ManualTextureServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CImage.h"
#include "CBLPImage.h"
#include "CD3D11Texture.h"
#include "CD3D11RenderTarget.h"
#include "CD3D11Driver.h"

CD3D11ManualTextureServices::CD3D11ManualTextureServices()
{
	Driver= static_cast<CD3D11Driver*>(g_Engine->getDriver());
	Driver->registerLostReset(this);

	loadDefaultTextures();
}

CD3D11ManualTextureServices::~CD3D11ManualTextureServices()
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

ITexture* CD3D11ManualTextureServices::addTexture( const c8* name, const dimension2du& size, IImage* img, bool mipmap )
{
	if(TextureMap.find(name) != TextureMap.end())
		return NULL_PTR;

	CD3D11Texture* tex = new CD3D11Texture(mipmap);
	if(!tex->createFromImage(size, img))
	{
		tex->drop();
		return NULL_PTR;
	}
	TextureMap[name] =  tex;
	return tex;
}

void CD3D11ManualTextureServices::removeTexture( const c8* name )
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

IRenderTarget* CD3D11ManualTextureServices::addRenderTarget( const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt )
{
	CD3D11RenderTarget* tex = new CD3D11RenderTarget(size, colorFmt, depthFmt);
	ASSERT(tex->isValid());

	RenderTargets.push_back(tex);

	return tex;
}

void CD3D11ManualTextureServices::removeRenderTarget( IRenderTarget* texture )
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

ITexture* CD3D11ManualTextureServices::createTextureFromImage( const dimension2du& size, IImage* image, bool mipmap )
{
	CD3D11Texture* tex = new CD3D11Texture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	return tex;
}

ITexture* CD3D11ManualTextureServices::createTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	CImage* image = new CImage(format, size, data, false);
	CD3D11Texture* tex = new CD3D11Texture(mipmap);
	if (!tex->createFromImage(size, image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	image->drop();
	return tex;
}

ITexture* CD3D11ManualTextureServices::createCompressTextureFromData( const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap )
{
	CBLPImage* image = new CBLPImage;
	if (!image->fromImageData((const u8*)data, size, format, mipmap))
	{
		image->drop();
		return NULL_PTR;
	}

	CD3D11Texture* tex = new CD3D11Texture(mipmap);
	if (!tex->createFromBlpImage(image))
	{
		tex->drop();
		tex = NULL_PTR;
	}
	image->drop();
	return tex;
}

ITexture* CD3D11ManualTextureServices::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	CD3D11Texture* tex = new CD3D11Texture(false);
	if (tex->createEmptyTexture(size, format))
	{
		ASSERT(tex->isValid());
		return tex;
	}
	tex->drop();
	return NULL_PTR;
}

void CD3D11ManualTextureServices::onLost()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onLost();
	}
}

void CD3D11ManualTextureServices::onReset()
{
	for (T_RTList::const_iterator itr = RenderTargets.begin(); itr != RenderTargets.end(); ++itr)
	{
		(*itr)->onReset();
	}
}

void CD3D11ManualTextureServices::loadDefaultTextures()
{
	ECOLOR_FORMAT format = ECF_A8R8G8B8;
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
