#include "stdafx.h"
#include "CD3D9Texture.h"
#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "mywow.h"


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
	: DXTexture(NULL), RTTSurface(NULL), DepthSurface(NULL),
	TextureSize(0,0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(false), IsRenderTarget(false), IsBLP(false)
{
	Image = NULL;
	BlpImage = NULL;
}

CD3D9Texture::CD3D9Texture( IImage* image, bool mipmap )
	: DXTexture(NULL), RTTSurface(NULL), DepthSurface(NULL),
	TextureSize(0,0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(mipmap), IsRenderTarget(false), IsBLP(false)
{
	Image = image;
	BlpImage = NULL;

	Image->grab();
}

CD3D9Texture::CD3D9Texture( IBLPImage* blpimage, bool mipmap )
	: DXTexture(NULL), RTTSurface(NULL), DepthSurface(NULL),
	TextureSize(0,0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(mipmap), IsRenderTarget(false), IsBLP(true)
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
	SAFE_RELEASE(DepthSurface);
	SAFE_RELEASE(RTTSurface);
	SAFE_RELEASE(DXTexture);

	VideoBuilt = false;
}


bool CD3D9Texture::createEmptyTexture( dimension2du size, ECOLOR_FORMAT format, bool mipmap )
{
	if (VideoBuilt)
		return false;

	if( DXTexture)
		return false;

	HasMipMaps = mipmap;

	D3DFORMAT d3dfmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	IDirect3DDevice9* device = (IDirect3DDevice9*)g_Engine->getDriver()->getD3DDevice();
	IDirect3DTexture9* tex = NULL;
	HRESULT hr = device->CreateTexture(size.Width, size.Height,
		mipmap ? 0 : 1,			//no mipmap
		0,		//lockable
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

	IDirect3DSurface9* surf = NULL;
	DXTexture->GetSurfaceLevel(0, &surf);
	D3DSURFACE_DESC desc;
	surf->GetDesc(&desc);
	SAFE_RELEASE(surf);

	TextureSize.Height = desc.Height;
	TextureSize.Width = desc.Width;

	ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(desc.Format);

	VideoBuilt = true;
	return true;
}

bool CD3D9Texture::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format )
{
	if (VideoBuilt)
		return false;

	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	TextureSize = size.getOptimalSize(!driver->queryFeature(EVDF_TEXTURE_NPOT), 
		!driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
		true, 
		driver->Caps.MaxTextureWidth);

	D3DFORMAT d3dformat;

	if(ColorFormat == ECF_UNKNOWN)
	{
		if (format != ECF_UNKNOWN)
		{
			ColorFormat = format;
			d3dformat =  CD3D9Helper::getD3DFormatFromColorFormat(ColorFormat);
		}
		else
		{
			ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(driver->BackBufferFormat);
			d3dformat = driver->BackBufferFormat;
		}
	}
	else
	{
		d3dformat = CD3D9Helper::getD3DFormatFromColorFormat(ColorFormat);
	}

	// create texture
	HRESULT hr;
	IDirect3DDevice9* device = (IDirect3DDevice9*)g_Engine->getDriver()->getD3DDevice();
	hr = device->CreateTexture(
		TextureSize.Width,
		TextureSize.Height,
		1, // mip map level count, we don't want mipmaps here
		D3DUSAGE_RENDERTARGET,
		d3dformat,
		D3DPOOL_DEFAULT,
		&DXTexture,
		NULL);

	if(FAILED(hr))
	{
		_ASSERT(false);
		return false;
	}

	//create depth buffer
	D3DFORMAT fmt = D3DFMT_D24S8;
	if (driver->DepthStencilSurface)
	{
		D3DSURFACE_DESC desc;
		driver->DepthStencilSurface->GetDesc(&desc);
		fmt = desc.Format;
	}

	D3DSURFACE_DESC desc2;
	DXTexture->GetLevelDesc(0, &desc2);
	hr = device->CreateDepthStencilSurface(TextureSize.Width,
		TextureSize.Height,
		fmt,
		desc2.MultiSampleType,
		desc2.MultiSampleQuality,
		FALSE,
		&DepthSurface,
		NULL);

	if(FAILED(hr))
	{
		_ASSERT(false);
		return false;
	}

	//create RTT Surface
	if (!RTTSurface)
	{
		hr = DXTexture->GetSurfaceLevel(0, &RTTSurface);

		_ASSERT( SUCCEEDED(hr) );
	}

	IsRenderTarget = true;

	VideoBuilt = true;
	return true;
}

void* CD3D9Texture::lock( bool readOnly /*= false*/, u32 mipmapLevel/*=0*/ )
{
	_ASSERT(DXTexture);

	MipLevelLocked = mipmapLevel;
	HRESULT hr;
	D3DLOCKED_RECT	rect;
	if ( !IsRenderTarget )
	{
		hr = DXTexture->LockRect( 0, &rect, 0, readOnly ? D3DLOCK_READONLY : 0 );
		_ASSERT( SUCCEEDED(hr) );
	}
	else
	{
		hr = RTTSurface->LockRect( &rect, 0, readOnly ? D3DLOCK_READONLY : 0 );

		_ASSERT( SUCCEEDED(hr) );
	}

	return rect.pBits;
}

void CD3D9Texture::unlock()
{
	_ASSERT(DXTexture);

	if ( !IsRenderTarget )
		DXTexture->UnlockRect(MipLevelLocked);
	else 
	{
		_ASSERT(RTTSurface);
		RTTSurface->UnlockRect();
	}
}

void CD3D9Texture::onLost()
{
	if ( IsRenderTarget )
		releaseVideoTexture();
}

void CD3D9Texture::onReset()
{
	if ( IsRenderTarget )
		createAsRenderTarget(TextureSize, ColorFormat);
}


bool CD3D9Texture::createTexture( dimension2du size, ECOLOR_FORMAT format, bool mipmap )
{
	TextureSize = size;

	D3DFORMAT d3dfmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	IDirect3DDevice9* device = (IDirect3DDevice9*)g_Engine->getDriver()->getD3DDevice();
	HRESULT hr = device->CreateTexture(TextureSize.Width, TextureSize.Height,
		mipmap ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		0, // usage
		d3dfmt, 
		D3DPOOL_MANAGED , 
		&DXTexture, NULL);

	ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(d3dfmt);

	return (SUCCEEDED(hr));
}

void CD3D9Texture::copyTexture( IBLPImage* blpimage )
{
	_ASSERT(blpimage);

	D3DLOCKED_RECT rect;
	HRESULT hr = DXTexture->LockRect(0, &rect, 0, 0);
	_ASSERT(SUCCEEDED(hr));

	blpimage->copyMipmapData(0, rect.pBits, rect.Pitch, TextureSize.Width, TextureSize.Height);

	DXTexture->UnlockRect(0);
}

bool CD3D9Texture::copyBlpMipMaps( u32 level /*= 1*/ )
{
	if (!HasMipMaps || !BlpImage)
		return false;

	if (level==0)
		return true;

	// manual mipmap generation
	IDirect3DSurface9* lowerSurface = 0;

	// get lower level
	HRESULT hr = DXTexture->GetSurfaceLevel(level, &lowerSurface);
	if (FAILED(hr) || !lowerSurface)
	{
		_ASSERT(false);
		return false;
	}

	D3DSURFACE_DESC lowerDesc;
	lowerSurface->GetDesc(&lowerDesc);

	D3DLOCKED_RECT lowerlr;

	// lock lower surface
	if (FAILED(lowerSurface->LockRect(&lowerlr, NULL, 0)))
	{
		lowerSurface->Release();
		return false;
	}

	BlpImage->copyMipmapData(level, lowerlr.pBits, lowerlr.Pitch, lowerDesc.Width, lowerDesc.Height);

	bool result=true;
	// unlock
	if (FAILED(lowerSurface->UnlockRect()))
		result=false;

	// release
	lowerSurface->Release();

	if (!result || (lowerDesc.Width < 2 && lowerDesc.Height < 2))
		return result; // stop generating levels

	// generate next level
	return copyBlpMipMaps(level+1);
}

void CD3D9Texture::copyTexture( IImage* image )
{
	_ASSERT(image);

	D3DLOCKED_RECT rect;
	HRESULT hr = DXTexture->LockRect(0, &rect, 0, 0);
	_ASSERT(SUCCEEDED(hr));

	image->copyToScaling(rect.pBits, TextureSize.Width, TextureSize.Height, ColorFormat, rect.Pitch);

	DXTexture->UnlockRect(0);
}

bool CD3D9Texture::createMipMaps( u32 level /*= 1 */ )
{
	if (!HasMipMaps || IsBLP)
		return false;

	if (level==0)
		return true;

	// manual mipmap generation
	IDirect3DSurface9* upperSurface = 0;
	IDirect3DSurface9* lowerSurface = 0;

	// get upper level
	HRESULT hr = DXTexture->GetSurfaceLevel(level-1, &upperSurface);
	if (FAILED(hr) || !upperSurface)
	{
		_ASSERT(false);
		return false;
	}

	// get lower level
	hr = DXTexture->GetSurfaceLevel(level, &lowerSurface);
	if (FAILED(hr) || !lowerSurface)
	{
		_ASSERT(false);
		upperSurface->Release();
		return false;
	}

	D3DSURFACE_DESC upperDesc, lowerDesc;
	upperSurface->GetDesc(&upperDesc);
	lowerSurface->GetDesc(&lowerDesc);

	D3DLOCKED_RECT upperlr;
	D3DLOCKED_RECT lowerlr;

	// lock upper surface
	if (FAILED(upperSurface->LockRect(&upperlr, NULL, 0)))
	{
		upperSurface->Release();
		lowerSurface->Release();
		return false;
	}

	// lock lower surface
	if (FAILED(lowerSurface->LockRect(&lowerlr, NULL, 0)))
	{
		upperSurface->UnlockRect();
		upperSurface->Release();
		lowerSurface->Release();
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
	if (FAILED(upperSurface->UnlockRect()))
		result=false;
	if (FAILED(lowerSurface->UnlockRect()))
		result=false;

	// release
	upperSurface->Release();
	lowerSurface->Release();

	if (!result || (lowerDesc.Width < 2 && lowerDesc.Height < 2))
		return result; // stop generating levels

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



