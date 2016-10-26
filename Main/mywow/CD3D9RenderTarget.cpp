#include "stdafx.h"
#include "CD3D9RenderTarget.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "CD3D9Texture.h"

CD3D9RenderTarget::CD3D9RenderTarget( const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt )
	: IRenderTarget(colorFmt, depthFmt), RTTSurface(NULL_PTR), DepthSurface(NULL_PTR)
{
	RTCopyTexture = new CD3D9Texture(false);

	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(size, colorFmt, depthFmt, setting.antialias, setting.quality);
	ASSERT(success);

	success = RTCopyTexture->createRTTexture(TextureSize, ColorFormat);
	ASSERT(success);
}

CD3D9RenderTarget::~CD3D9RenderTarget()
{
	RTCopyTexture->drop();

	releaseVideoTexture();
}

ITexture* CD3D9RenderTarget::getRTTexture() const
{
	return RTCopyTexture;
}

bool CD3D9RenderTarget::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, u32 antialias, u32 quality )
{
	if (VideoBuilt)
		return false;

	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	TextureSize = size;
	// 		size.getOptimalSize(!driver->queryFeature(EVDF_TEXTURE_NPOT), 
	// 		!driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
	// 		true, 
	// 		driver->Caps.MaxTextureWidth);

	ColorFormat = colorFmt;

	if(ColorFormat == ECF_UNKNOWN)
	{
		ColorFormat = CD3D9Helper::getColorFormatFromD3DFormat(driver->BackBufferFormat);
	}

	D3DFORMAT cfmt =  CD3D9Helper::getD3DFormatFromColorFormat(ColorFormat);

	// create render target
	HRESULT hr;
	IDirect3DDevice9* device = (IDirect3DDevice9*)driver->pID3DDevice;
	hr = device->CreateRenderTarget(
		TextureSize.Width,
		TextureSize.Height,
		cfmt,
		(D3DMULTISAMPLE_TYPE)antialias,
		quality,
		FALSE,			//not lockable
		&RTTSurface,
		NULL_PTR);

	if(FAILED(hr))
	{
		ASSERT(false);
		return false;
	}

	//create depth buffer
	if (DepthFormat == ECF_UNKNOWN)
	{
		if (driver->DefaultDepthBuffer)
		{
			D3DSURFACE_DESC desc;
			driver->DefaultDepthBuffer->GetDesc(&desc);
			DepthFormat = CD3D9Helper::getColorFormatFromD3DFormat(desc.Format);
		}
	}

	D3DFORMAT dfmt = CD3D9Helper::getD3DFormatFromColorFormat(DepthFormat);

	hr = device->CreateDepthStencilSurface(TextureSize.Width,
		TextureSize.Height,
		dfmt,
		(D3DMULTISAMPLE_TYPE)antialias,
		quality,
		FALSE,
		&DepthSurface,
		NULL_PTR);

	if(FAILED(hr))
	{
		ASSERT(false);
		return false;
	}

	VideoBuilt = true;
	return true;
}

void CD3D9RenderTarget::releaseVideoTexture()
{
	SAFE_RELEASE(DepthSurface);
	SAFE_RELEASE(RTTSurface);

	VideoBuilt = false;
}

void CD3D9RenderTarget::onLost()
{
	RTCopyTexture->releaseVideoTexture();
	
	releaseVideoTexture();
}

void CD3D9RenderTarget::onReset()
{
	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(TextureSize, ColorFormat, DepthFormat, setting.antialias, setting.quality);
	ASSERT(success);

	RTCopyTexture->createRTTexture(TextureSize, ColorFormat);
}

bool CD3D9RenderTarget::writeToRTTexture()
{
	IDirect3DSurface9* surface = NULL_PTR;
	HRESULT hr = RTCopyTexture->getDXTexture()->GetSurfaceLevel(0, &surface);

	if (FAILED(hr))
	{
		ASSERT(false);
		SAFE_RELEASE(surface);
		return false;
	}

	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	IDirect3DDevice9* device = (IDirect3DDevice9*)driver->pID3DDevice;

	hr = device->StretchRect(RTTSurface, NULL_PTR, surface, NULL_PTR, D3DTEXF_NONE);
	if (FAILED(hr))
	{
		ASSERT(false);
		SAFE_RELEASE(surface);
		return false;
	}

	SAFE_RELEASE(surface);
	return true;
}

#endif