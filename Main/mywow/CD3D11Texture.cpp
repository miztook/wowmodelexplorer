#include "stdafx.h"
#include "CD3D11Texture.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Helper.h"
#include "CD3D11Driver.h"
#include "CBlit.h"


CD3D11Texture::CD3D11Texture( bool mipmap )
	: DXTexture(NULL_PTR), SRView(NULL_PTR)
{
	HasMipMaps = mipmap;
}

CD3D11Texture::~CD3D11Texture()
{
}

bool CD3D11Texture::createVideoTexture()
{
	//CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	IBLPImage* blpImage = g_Engine->getResourceLoader()->loadBLP(getFileName());
	if(!blpImage || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	if (!createTexture(blpImage, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11Texture::createVideoTexture Failed");
		
		blpImage->drop();

		ASSERT(false);
		return false;
	}

	blpImage->drop();

	VideoBuilt = true;

	return true;
}

void CD3D11Texture::releaseVideoTexture()
{
	//CLock lock(&g_Globals.textureCS);

	SAFE_RELEASE_STRICT(SRView);
	SAFE_RELEASE_STRICT(DXTexture);
	
	VideoBuilt = false;
}

bool CD3D11Texture::createFromImage( const dimension2du& size, IImage* image )
{
	//CLock lock(&g_Globals.textureCS);

	if(VideoBuilt || !image || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	if (!createTexture(size, image, HasMipMaps))
	{
		ASSERT(false);
		return false;
	}

	VideoBuilt = true;

	return true;
}

bool CD3D11Texture::createFromBlpImage( IBLPImage* blpImage )
{
	//CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	if(!blpImage || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	if (!createTexture(blpImage, HasMipMaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D11Texture::createFromBlpImage Failed");
		ASSERT(false);
		return false;
	}

	VideoBuilt = true;

	return true;
}

bool CD3D11Texture::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	//CLock lock(&g_Globals.textureCS);
	
	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	HasMipMaps = false;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;

	DXGI_FORMAT dxgifmt = CD3D11Helper::getDXGIFormatFromColorFormat(format);

	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	ID3D11Device* pDevice = driver->pID3DDevice11;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	desc.ArraySize = 1;
	desc.CPUAccessFlags = 0;
	desc.Format = dxgifmt;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = TextureSize.Width;
	desc.Height = TextureSize.Height;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;

	u32 numberOfMipLevels = desc.MipLevels;
	
	HRESULT hr = pDevice->CreateTexture2D(&desc, NULL_PTR, &DXTexture);
	if (FAILED(hr))
	{
		ASSERT(false);
		return false;
	}

	SampleCount = (u8)desc.SampleDesc.Count;

	//create resource view
	if (!createViews(dxgifmt, numberOfMipLevels, SampleCount > 1))
	{
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

bool CD3D11Texture::createRTTexture( const dimension2du& size, ECOLOR_FORMAT format, u32 antialias, u32 quality )
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	Type = ETT_RENDERTARGET;
	HasMipMaps = false;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;

	DXGI_FORMAT dxgifmt = CD3D11Helper::getDXGIFormatFromColorFormat(format);

	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	ID3D11Device* pDevice = driver->pID3DDevice11;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	desc.ArraySize = 1;
	desc.CPUAccessFlags = 0;
	desc.Format = dxgifmt;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = antialias;
	desc.SampleDesc.Quality = quality;
	desc.Width = TextureSize.Width;
	desc.Height = TextureSize.Height;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MipLevels = 1;		//no mipmap

	u32 numberOfMipLevels = desc.MipLevels;

	HRESULT hr = pDevice->CreateTexture2D(&desc, NULL_PTR, &DXTexture);
	if (FAILED(hr))
	{
		ASSERT(false);
		return false;
	}

	SampleCount = (u8)desc.SampleDesc.Count;

	//create resource view
	if (!createViews(dxgifmt, numberOfMipLevels, SampleCount > 1))
	{
		ASSERT(false);
		return false;
	}

	VideoBuilt = true;

	return true;
}

bool CD3D11Texture::createDSTexture( const dimension2du& size, ECOLOR_FORMAT format, u32 antialias, u32 quality )
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	Type = ETT_DEPTHSTENCIL;
	HasMipMaps = false;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;

	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	ID3D11Device* pDevice = driver->pID3DDevice11;

	DXGI_FORMAT dxgifmt;
	DXGI_FORMAT srfmt;
	
	if (ColorFormat == ECF_UNKNOWN)
	{
		dxgifmt = driver->DepthTextureFormat;
		srfmt = driver->DepthSRFormat; 
	}
	else
	{
		CD3D11Helper::getDXGIFormatFromColorFormat(ColorFormat, dxgifmt, srfmt);
	}

	D3D11_TEXTURE2D_DESC desc;
	::memset( &desc, 0, sizeof( desc ) );
	desc.ArraySize = 1;
	desc.BindFlags = driver->FeatureLevel >= D3D_FEATURE_LEVEL_10_1 ? (D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE) : D3D11_BIND_DEPTH_STENCIL;
	desc.Format = dxgifmt;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = antialias;
	desc.SampleDesc.Quality = quality;
	desc.Width = (UINT)TextureSize.Width;
	desc.Height = (UINT)TextureSize.Height;
	HRESULT hr = pDevice->CreateTexture2D( &desc, NULL_PTR, &DXTexture );
	if(FAILED(hr))
	{
		ASSERT(false);
		return NULL_PTR;
	}

	SampleCount = desc.SampleDesc.Count;

	u32 numberOfMipLevels = desc.MipLevels;

	if (driver->FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
	{
		//create resource view
		if (!createViews(srfmt, numberOfMipLevels, antialias > 1))
		{
			ASSERT(false);
			return false;
		}
	}
	
	VideoBuilt = true;

	return true;
}

bool CD3D11Texture::createViews( DXGI_FORMAT format, u32 mipmapLevels, bool multisample )
{
	if (!DXTexture || SRView)
	{
		ASSERT(false);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	::memset(&desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.Format = format;

	desc.ViewDimension = multisample ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = mipmapLevels;
	desc.Texture2D.MostDetailedMip = 0;

	ID3D11Device* pDevice = static_cast<CD3D11Driver*>(g_Engine->getDriver())->pID3DDevice11;
	HRESULT hr = pDevice->CreateShaderResourceView(DXTexture, &desc, &SRView);

	return (SUCCEEDED(hr));
}

bool CD3D11Texture::createTexture( const dimension2du& size, IImage* image, bool mipmap )
{
	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = mipmap ? image->getDimension().getNumMipLevels() : 1;
	TextureSize = size;	//image->getDimension();
	ColorFormat = image->getColorFormat();

	DXGI_FORMAT dxgifmt = CD3D11Helper::getDXGIFormatFromColorFormat(ColorFormat);

	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	ID3D11Device* pDevice = driver->pID3DDevice11;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	desc.ArraySize = 1;
	desc.CPUAccessFlags = 0;
	desc.Format = dxgifmt;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = TextureSize.Width;
	desc.Height = TextureSize.Height;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = NumMipmaps;

	D3D11_SUBRESOURCE_DATA* initData = (D3D11_SUBRESOURCE_DATA*)Z_AllocateTempMemory(sizeof(D3D11_SUBRESOURCE_DATA) * NumMipmaps);
	buildTempSysMemData(initData, NumMipmaps, image);

	HRESULT hr = pDevice->CreateTexture2D(&desc, initData, &DXTexture);
	if (FAILED(hr))
	{
		ASSERT(false);
		return false;
	}

	if (FAILED(hr))
	{
		destroyTempSysMemData(initData, NumMipmaps);
		Z_FreeTempMemory(initData);
		ASSERT(false);
		return false;
	}

	destroyTempSysMemData(initData, NumMipmaps);
	Z_FreeTempMemory(initData);

	ColorFormat = CD3D11Helper::getColorFormatFromDXGIFormat(dxgifmt);
	SampleCount = desc.SampleDesc.Count;

	//create resource view
	if (!createViews(dxgifmt, NumMipmaps, SampleCount > 1))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

bool CD3D11Texture::createTexture( IBLPImage* blpimage, bool mipmap )
{
	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = mipmap ? blpimage->getNumMipLevels() : 1;
	TextureSize = blpimage->getDimension();
	ColorFormat = blpimage->getColorFormat();

	DXGI_FORMAT dxgifmt = CD3D11Helper::getDXGIFormatFromColorFormat(ColorFormat);

	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	ID3D11Device* pDevice = driver->pID3DDevice11;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	desc.ArraySize = 1;
	desc.CPUAccessFlags = 0;
	desc.Format = dxgifmt;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = TextureSize.Width;
	desc.Height = TextureSize.Height;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	desc.MipLevels = NumMipmaps;

	D3D11_SUBRESOURCE_DATA* initData = (D3D11_SUBRESOURCE_DATA*)Z_AllocateTempMemory(sizeof(D3D11_SUBRESOURCE_DATA) * NumMipmaps);
	buildTempSysMemData(initData, NumMipmaps, blpimage);

	HRESULT hr = pDevice->CreateTexture2D(&desc, initData, &DXTexture);
	if (FAILED(hr))
	{
		destroyTempSysMemData(initData, NumMipmaps);
		Z_FreeTempMemory(initData);
		ASSERT(false);
		return false;
	}

	destroyTempSysMemData(initData, NumMipmaps);
	Z_FreeTempMemory(initData);

	ColorFormat = CD3D11Helper::getColorFormatFromDXGIFormat(dxgifmt);
	SampleCount = desc.SampleDesc.Count;

	//create resource view
	if (!createViews(dxgifmt, NumMipmaps, SampleCount > 1))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void CD3D11Texture::buildTempSysMemData( D3D11_SUBRESOURCE_DATA* subData, u32 size, IBLPImage* blpimage )
{
	ASSERT(size && size <= NumMipmaps);

	for (u32 i=0; i<size; ++i)
	{
		dimension2du mipSize = TextureSize.getMipLevelSize(i);

		u32 pitch, bytes;
		getImagePitchAndBytes(ColorFormat, mipSize.Width, mipSize.Height, pitch, bytes);

		void* dest = Z_AllocateTempMemory(bytes);
		if(!blpimage->copyMipmapData( i, dest, pitch, mipSize.Width, mipSize.Height ))
		{
			ASSERT(false);
			break;
		}	

		subData[i].pSysMem = dest;
		subData[i].SysMemPitch = pitch;
		subData[i].SysMemSlicePitch = 0;
	}
}

void CD3D11Texture::buildTempSysMemData( D3D11_SUBRESOURCE_DATA* subData, u32 size, IImage* image )
{
	ASSERT(size && size <= NumMipmaps);

	u32 bytesPerPixel = getBytesPerPixelFromFormat(ColorFormat);

	//0
	dimension2du mipSize = TextureSize.getMipLevelSize(0);
	u32 pitch = mipSize.Width * bytesPerPixel;
	void* dest = Z_AllocateTempMemory(pitch * mipSize.Height);
	
	image->copyToScaling(dest, mipSize.Width, mipSize.Height, ColorFormat, pitch);

	subData[0].pSysMem = dest;
	subData[0].SysMemPitch = pitch;
	subData[0].SysMemSlicePitch = 0;

	for(u32 i=1; i<size; ++i)
	{
		dimension2du upperSize = TextureSize.getMipLevelSize(i - 1);
		dimension2du lowerSize = TextureSize.getMipLevelSize(i);

		u32 upperPitch = upperSize.Width * bytesPerPixel;
		u32 lowerPitch = lowerSize.Width * bytesPerPixel;
		u8* destdata = (u8*)Z_AllocateTempMemory(pitch * lowerSize.Height);
		const u8* src = (u8*)subData[i-1].pSysMem;

		if (ColorFormat == ECF_R5G6B5 || ColorFormat == ECF_A1R5G5B5)
			CBlit::copy16BitMipMap(src, destdata,
			lowerSize.Width, lowerSize.Height,
			upperPitch, lowerPitch, ColorFormat);
		else if (ColorFormat == ECF_A8R8G8B8)
			CBlit::copy32BitMipMap(src, destdata,
			lowerSize.Width, lowerSize.Height,
			upperPitch, lowerPitch, ColorFormat);
		else
			ASSERT(false);				//不支持的格式

		subData[i].pSysMem = destdata;
		subData[i].SysMemPitch = lowerPitch;
		subData[i].SysMemSlicePitch = 0;
	}
}

void CD3D11Texture::destroyTempSysMemData( D3D11_SUBRESOURCE_DATA* subData, u32 size )
{
	for (s32 i=(s32)size-1; i>=0; --i)
	{
		void* data = (void*)subData[i].pSysMem;
		Z_FreeTempMemory(data);
	}
}

#endif