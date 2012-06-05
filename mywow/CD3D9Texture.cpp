#include "stdafx.h"
#include "CD3D9Texture.h"
#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "mywow.h"
#include "CImage.h"

#define  MAX_TEXTURE_SIZE	1024

CD3D9Texture::CD3D9Texture()
	: DXTexture(NULL), RTTSurface(NULL), DepthSurface(NULL),
	ImageSize(0,0), TextureSize(0,0), Pitch(0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(false), IsRenderTarget(false)
{
	Image = 0;

	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	Device = Driver->pID3DDevice;
}

CD3D9Texture::CD3D9Texture( IImage* image, bool mipmap )
	: DXTexture(NULL), RTTSurface(NULL), DepthSurface(NULL),
	ImageSize(0,0), TextureSize(0,0), Pitch(0), ColorFormat(ECF_UNKNOWN),
	HasMipMaps(mipmap), IsRenderTarget(false)
{
	Image = image;

	Image->grab();

	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	Device = Driver->pID3DDevice;
}

CD3D9Texture::~CD3D9Texture()
{
	releaseTexture();

	if (Image)
		Image->drop();
}

bool CD3D9Texture::createFromImage()
{
	if (VideoBuilt)
		return false;

	_ASSERT( g_Engine->getCurrentThreadId() == ::GetCurrentThreadId() );

	if(!Image || DXTexture)
		return false;

	if (!createTexture(Image, MAX_TEXTURE_SIZE, HasMipMaps))
		return false;

	copyTexture(Image);

	if (HasMipMaps)
		createMipMaps();				//填充mipmap

	VideoBuilt = true;
	return true;

	return true;
}

void CD3D9Texture::releaseTexture()
{
	SAFE_RELEASE(DepthSurface);
	SAFE_RELEASE(RTTSurface);
	SAFE_RELEASE(DXTexture);

	VideoBuilt = false;
}

bool CD3D9Texture::createFromDXTexture( IDirect3DTexture9* d3d9Texture )
{
	if (VideoBuilt)
		return false;

	if( DXTexture || !d3d9Texture)
		return false;

	DXTexture = d3d9Texture;

	IDirect3DSurface9* surf = NULL;
	DXTexture->GetSurfaceLevel(0, &surf);
	D3DSURFACE_DESC desc;
	surf->GetDesc(&desc);
	SAFE_RELEASE(surf);

	TextureSize.Height = desc.Height;
	TextureSize.Width = desc.Width;
	ImageSize = TextureSize;

	ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(desc.Format);
	setPitch(desc.Format);

	VideoBuilt = true;
	return true;
}

bool CD3D9Texture::createEmptyTexture( dimension2du size, ECOLOR_FORMAT format, bool mipmap )
{
	if (VideoBuilt)
		return false;

	if( DXTexture)
		return false;

	HasMipMaps = mipmap;

	D3DFORMAT d3dfmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	IDirect3DTexture9* tex = NULL;
	HRESULT hr = Device->CreateTexture(size.Width, size.Height,
		mipmap ? 0 : 1,			//no mipmap
		g_Engine->IsVista() ? D3DUSAGE_DYNAMIC : 0,		//lockable
		d3dfmt,
		g_Engine->IsVista() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
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
	ImageSize = TextureSize;

	ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(desc.Format);
	setPitch(desc.Format);

	VideoBuilt = true;
	return true;
}

bool CD3D9Texture::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format )
{
	if (VideoBuilt)
		return false;

	ImageSize = size;
	TextureSize = size.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT), 
		!Driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
		true, 
		Driver->Caps.MaxTextureWidth);

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
			ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(Driver->BackBufferFormat);
			d3dformat = Driver->BackBufferFormat;
		}

		setPitch(d3dformat); 
		_ASSERT(Pitch!=0);
	}
	else
	{
		d3dformat = CD3D9Helper::getD3DFormatFromColorFormat(ColorFormat);
	}

	// create texture
	HRESULT hr;

	hr = Device->CreateTexture(
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
	if (Driver->DepthStencilSurface)
	{
		D3DSURFACE_DESC desc;
		Driver->DepthStencilSurface->GetDesc(&desc);
		fmt = desc.Format;
	}

	D3DSURFACE_DESC desc2;
	DXTexture->GetLevelDesc(0, &desc2);
	hr = Device->CreateDepthStencilSurface(TextureSize.Width,
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
		releaseTexture();
}

void CD3D9Texture::onReset()
{
	if ( IsRenderTarget )
		createAsRenderTarget(TextureSize, ColorFormat);
}


bool CD3D9Texture::createTexture( IImage* image, u32 maxTextureSize, bool mipmap )
{
	ImageSize = image->getDimension();

	dimension2du optSize = ImageSize.getOptimalSize(
		!Driver->queryFeature(EVDF_TEXTURE_NPOT), 
		!Driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
		true, 
		Driver->Caps.MaxTextureWidth);

	if (optSize.Height > maxTextureSize ||
		optSize.Width > maxTextureSize )
	{
		optSize.set(maxTextureSize, maxTextureSize );
	}

	D3DFORMAT format = CD3D9Helper::getD3DFormatFromColorFormat(image->getColorFormat());

	DWORD usage = 0;

	/*
	// This enables hardware mip map generation.
	if (mipmap && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))			//目前都不支持硬件自动生成mipmap
	{
		LPDIRECT3D9 intf = Driver->pID3D;
		D3DDISPLAYMODE d3ddm;
		intf->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		if (D3D_OK==intf->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,d3ddm.Format,D3DUSAGE_AUTOGENMIPMAP,D3DRTYPE_TEXTURE,format))
		{
			usage = D3DUSAGE_AUTOGENMIPMAP;
			HardwareMipMaps = true;
		}
	}
	*/

	HRESULT hr = Device->CreateTexture(optSize.Width, optSize.Height,
		mipmap ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		g_Engine->IsVista() ? usage | D3DUSAGE_DYNAMIC : usage, // usage
		format, 
		g_Engine->IsVista() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED , 
		&DXTexture, NULL);

	TextureSize.Width = optSize.Width;
	TextureSize.Height = optSize.Height;

	ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(format);
	setPitch(format);

	return (SUCCEEDED(hr));
}

void CD3D9Texture::copyTexture( IImage* image )
{
	_ASSERT(image);

	D3DSURFACE_DESC desc;
	DXTexture->GetLevelDesc(0, &desc);

	TextureSize.Width = desc.Width;
	TextureSize.Height = desc.Height;

	D3DLOCKED_RECT rect;
	HRESULT hr = DXTexture->LockRect(0, &rect, 0, 0);
	_ASSERT(SUCCEEDED(hr));

	Pitch = rect.Pitch;
	image->copyToScaling(rect.pBits, TextureSize.Width, TextureSize.Height, ColorFormat, Pitch);

	DXTexture->UnlockRect(0);
}

bool CD3D9Texture::createMipMaps( u32 level /*= 1 */ )
{
	if (!HasMipMaps)
		return false;

	if (level==0)
		return true;

// 	if (HardwareMipMaps && DXTexture)
// 	{
// 		// generate mipmaps in hardware
// 		DXTexture->GenerateMipSubLevels();
// 		return true;
// 	}

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

	if (!result || (upperDesc.Width <= 3 && upperDesc.Height <= 3))
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

void CD3D9Texture::setPitch( D3DFORMAT d3dformat )
{
	switch(d3dformat)
	{
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		Pitch = TextureSize.Width * 2;
		break;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		Pitch = TextureSize.Width * 4;
		break;
	case D3DFMT_R5G6B5:
		Pitch = TextureSize.Width * 2;
		break;
	case D3DFMT_R8G8B8:
		Pitch = TextureSize.Width * 3;
		break;
	case D3DFMT_A8L8:
		Pitch = TextureSize.Width * 2;
		break;
	case D3DFMT_A8:
		Pitch = TextureSize.Width;
		break;
	default:
		//_ASSERT(false);	//dds无效
		Pitch = 0;
	};
}


