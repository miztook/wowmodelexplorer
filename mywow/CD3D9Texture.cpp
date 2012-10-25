#include "stdafx.h"
#include "CD3D9Texture.h"
#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "mywow.h"
#include "CLock.h"

extern CRITICAL_SECTION g_textureCS;

ITexture* CD3D9TexLoader::loadTexture( IImage* image, bool mipmap )
{
	ITexture* tex = new CD3D9Texture(image, mipmap);
	return tex;
}

ITexture* CD3D9TexLoader::loadTexture( IBLPImage* blpimage, bool mipmap )
{
	ITexture* tex = new CD3D9Texture(blpimage, mipmap);
	return tex;
}

CD3D9Texture::CD3D9Texture()
	: DXTexture(NULL), TextureSize(0,0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(false), IsBLP(false)
{
	Image = NULL;
	BlpImage = NULL;
}

CD3D9Texture::CD3D9Texture( IImage* image, bool mipmap )
	: DXTexture(NULL), TextureSize(0,0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(mipmap), IsBLP(false)
{
	Image = image;
	BlpImage = NULL;

	Image->grab();
}

CD3D9Texture::CD3D9Texture( IBLPImage* blpimage, bool mipmap )
	: DXTexture(NULL), TextureSize(0,0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(mipmap), IsBLP(true)
{
	BlpImage = blpimage;
	Image = NULL;

	BlpImage->grab();
}

CD3D9Texture::~CD3D9Texture()
{
	releaseVideoTexture();

	if (BlpImage)
		BlpImage->drop();

	if (Image)
		Image->drop();
}

bool CD3D9Texture::createVideoTexture()
{
	CLock lock(&g_textureCS);

	if (VideoBuilt)
		return true;

	if (IsBLP)
	{
		if(!BlpImage || DXTexture)
		{
			_ASSERT(false);
			return false;
		}

		if (!createTexture(BlpImage->getDimension(), BlpImage->getColorFormat(), HasMipMaps))
		{
			_ASSERT(false);
			return false;
		}

		copyTexture(BlpImage);

		if (HasMipMaps)
			copyBlpMipMaps();
	}
	else
	{
		if(!Image || DXTexture)
		{
			_ASSERT(false);
			return false;
		}

		if (!createTexture(Image->getDimension(), Image->getColorFormat(), HasMipMaps))
		{
			_ASSERT(false);
			return false;
		}

		copyTexture(Image);

		if (HasMipMaps)
			createMipMaps();				//填充mipmap
	}
	
	VideoBuilt = true;
	return true;
}

void CD3D9Texture::releaseVideoTexture()
{
	CLock lock(&g_textureCS);

	SAFE_RELEASE(DXTexture);
	VideoBuilt = false;
}

bool CD3D9Texture::createEmptyTexture( dimension2du size, ECOLOR_FORMAT format )
{
	if (VideoBuilt)
		return false;

	if( DXTexture)
		return false;

	HasMipMaps = false;
	TextureSize = size;
	ColorFormat = format;

	D3DFORMAT d3dfmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	IDirect3DDevice9* device = (IDirect3DDevice9*)g_Engine->getDriver()->getD3DDevice();
	IDirect3DTexture9* tex = NULL;
	HRESULT hr = device->CreateTexture(TextureSize.Width, TextureSize.Height,
		1,			//no mipmap
		0,			//no lock
		d3dfmt,
		D3DPOOL_MANAGED,
		&tex,
		NULL);

	if (FAILED(hr))
	{
		_ASSERT(false);
		return false;
	}

	DXTexture = tex;

	VideoBuilt = true;
	return true;
}

void* CD3D9Texture::lock( bool readOnly /*= false*/)
{
	D3DLOCKED_RECT rect;
	if (FAILED(DXTexture->LockRect(0, &rect, NULL, readOnly ? D3DLOCK_READONLY : 0 )))
		return NULL;

	return rect.pBits;
}

void CD3D9Texture::unlock( )
{
	DXTexture->UnlockRect(0);
}

bool CD3D9Texture::createTexture( dimension2du size, ECOLOR_FORMAT format, bool mipmap )
{
	TextureSize = size;

	D3DFORMAT d3dfmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	IDirect3DDevice9* device = (IDirect3DDevice9*)g_Engine->getDriver()->getD3DDevice();
	HRESULT hr = device->CreateTexture(TextureSize.Width, TextureSize.Height,
		mipmap ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		0,		//
		d3dfmt, 
		D3DPOOL_MANAGED , 
		&DXTexture, NULL);

	ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(d3dfmt);

	return (SUCCEEDED(hr));
}

void CD3D9Texture::copyTexture( IBLPImage* blpimage )
{
	_ASSERT(blpimage);

	D3DSURFACE_DESC desc;
	DXTexture->GetLevelDesc(0, &desc);

	D3DLOCKED_RECT rect;
	if (FAILED(DXTexture->LockRect(0, &rect, NULL, 0 )))
		return;

	bool result = blpimage->copyMipmapData(0, rect.pBits, rect.Pitch, desc.Width, desc.Height);
	_ASSERT(result);

	DXTexture->UnlockRect(0);
}

bool CD3D9Texture::copyBlpMipMaps( u32 level /*= 1*/ )
{
	if (!HasMipMaps || !BlpImage)
		return false;

	if (level==0)
		return true;

	D3DSURFACE_DESC lowerDesc;
	DXTexture->GetLevelDesc(level, &lowerDesc);

	D3DLOCKED_RECT lowerlr;
	if (FAILED(DXTexture->LockRect(level, &lowerlr, NULL, 0)))
		return false;

	bool result = BlpImage->copyMipmapData(level, lowerlr.pBits, lowerlr.Pitch, lowerDesc.Width, lowerDesc.Height);

	// unlock
	if (FAILED(DXTexture->UnlockRect(level)))
		result=false;

	if (!result || (lowerDesc.Width < 2 && lowerDesc.Height < 2))
		return false; // stop generating levels

	// generate next level
	return copyBlpMipMaps(level+1);
}

void CD3D9Texture::copyTexture( IImage* image )
{
	_ASSERT(image);

	D3DSURFACE_DESC desc;
	DXTexture->GetLevelDesc(0, &desc);

	D3DLOCKED_RECT rect;
	if (FAILED(DXTexture->LockRect(0, &rect, NULL, 0 )))
		return;

	image->copyToScaling(rect.pBits, desc.Width, desc.Height, ColorFormat, rect.Pitch);

	DXTexture->UnlockRect(0);
}

bool CD3D9Texture::createMipMaps( u32 level /*= 1 */ )
{
	if (!HasMipMaps || IsBLP)
		return false;

	if (level==0)
		return true;

	D3DSURFACE_DESC upperDesc, lowerDesc;
	DXTexture->GetLevelDesc(level-1, &upperDesc);
	DXTexture->GetLevelDesc(level, &lowerDesc);

	D3DLOCKED_RECT upperlr;
	D3DLOCKED_RECT lowerlr;

	// lock upper surface
	if (FAILED(DXTexture->LockRect(level-1, &upperlr, NULL, 0)))
		return false;

	// lock lower surface
	if (FAILED(DXTexture->LockRect(level, &lowerlr, NULL, 0)))
	{
		DXTexture->UnlockRect(level-1);
		return false;
	}

	if (upperDesc.Format != lowerDesc.Format)
	{
		_ASSERT(false);
	}
	else
	{
		if ((upperDesc.Format == D3DFMT_A1R5G5B5) || (upperDesc.Format == D3DFMT_R5G6B5))
			copy16BitMipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
			lowerDesc.Width, lowerDesc.Height,
			upperlr.Pitch, lowerlr.Pitch);
		else if (upperDesc.Format == D3DFMT_A8R8G8B8)
			copy32BitMipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
			lowerDesc.Width, lowerDesc.Height,
			upperlr.Pitch, lowerlr.Pitch);
		else if (upperDesc.Format == D3DFMT_A8L8)
			copyA8L8MipMap((char*)upperlr.pBits, (char*)lowerlr.pBits,
			lowerDesc.Width, lowerDesc.Height,
			upperlr.Pitch, lowerlr.Pitch);
		else
			_ASSERT(false);				//不支持的格式
	}

	bool result=true;
	// unlock
	if (FAILED(DXTexture->UnlockRect(level-1)))
		result=false;
	if (FAILED(DXTexture->UnlockRect(level)))
		result=false;

	if (!result || (lowerDesc.Width < 2 && lowerDesc.Height < 2))
		return false; // stop generating levels

	// generate next level
	return createMipMaps(level+1);
}

void CD3D9Texture::copyA8L8MipMap( char* src, char* tgt, s32 width, s32 height, s32 pitchsrc, s32 pitchtgt ) const
{
	_ASSERT(ColorFormat == ECF_A8L8);

	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			u32 a=0, l = 0;

			for (s32 dy=0; dy<2; ++dy)
			{
				const s32 tgy = (y*2)+dy;
				for (s32 dx=0; dx<2; ++dx)
				{
					const s32 tgx = (x*2)+dx;

					u16 c = *(u16*)(&src[(tgx*2)+(tgy*pitchsrc)]);

					a += (c & 0xff00) >> 8;
					l += (c & 0xff);
				}
			}

			a /= 4;
			l /= 4;

			u16 c = a<<8 | l; 
			*(u16*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
	}
}

void CD3D9Texture::copy16BitMipMap( char* src, char* tgt, s32 width, s32 height, s32 pitchsrc, s32 pitchtgt ) const
{
	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			u32 a=0, r=0, g=0, b=0;

			for (s32 dy=0; dy<2; ++dy)
			{
				const s32 tgy = (y*2)+dy;
				for (s32 dx=0; dx<2; ++dx)
				{
					const s32 tgx = (x*2)+dx;

					SColor c;
					if (ColorFormat == ECF_A1R5G5B5)
						c = SColor::A1R5G5B5toA8R8G8B8(*(u16*)(&src[(tgx*2)+(tgy*pitchsrc)]));
					else
						c = SColor::R5G6B5toA8R8G8B8(*(u16*)(&src[(tgx*2)+(tgy*pitchsrc)]));
	
					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			u16 c;
			if (ColorFormat == ECF_A1R5G5B5)
				c = SColor::RGBA16(r,g,b,a);
			else
				c = SColor::A8R8G8B8toR5G6B5(SColor(a,r,g,b).color);
			*(u16*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
	}
}

void CD3D9Texture::copy32BitMipMap( char* src, char* tgt, s32 width, s32 height, s32 pitchsrc, s32 pitchtgt ) const
{
	for (s32 y=0; y<height; ++y)
	{
		for (s32 x=0; x<width; ++x)
		{
			u32 a=0, r=0, g=0, b=0;
			SColor c;

			for (s32 dy=0; dy<2; ++dy)
			{
				const s32 tgy = (y*2)+dy;
				for (s32 dx=0; dx<2; ++dx)
				{
					const s32 tgx = (x*2)+dx;

					c = *(u32*)(&src[(tgx*4)+(tgy*pitchsrc)]);

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			c.set(a, r, g, b);
			*(u32*)(&tgt[(x*4)+(y*pitchtgt)]) = c.color;
		}
	}
}



