#include "stdafx.h"
#include "CD3D11RenderTarget.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Helper.h"
#include "CD3D11Driver.h"
#include "CD3D11Texture.h"

CD3D11RenderTarget::CD3D11RenderTarget(const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt)
	: IRenderTarget(colorFmt, depthFmt), RTView(NULL_PTR), DepthView(NULL_PTR)
{
	RTTexture = new CD3D11Texture(false);
	DSTexture = new CD3D11Texture(false);

	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(size, colorFmt, depthFmt, setting.antialias * 2, setting.quality);
	ASSERT(success);

	RTCopyTexture = new CD3D11Texture(false);
	success = createCopyTextures(TextureSize, ColorFormat);
	ASSERT(success);
}

CD3D11RenderTarget::~CD3D11RenderTarget()
{
	//DSCopyTexture->drop();
	RTCopyTexture->drop();

	DSTexture->drop();
	RTTexture->drop();

	releaseVideoTexture();
}

bool CD3D11RenderTarget::isValid() const
{
	return RTTexture->isValid() && DSTexture->isValid();
}

ITexture* CD3D11RenderTarget::getRTTexture() const
{
	return RTCopyTexture;
}

bool CD3D11RenderTarget::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, u32 antialias, u32 quality )
{
	if (VideoBuilt)
		return false;

	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	
	TextureSize = size;
	// 		size.getOptimalSize(!driver->queryFeature(EVDF_TEXTURE_NPOT), 
	// 		!driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
	// 		true, 
	// 		driver->Caps.MaxTextureWidth);

	ColorFormat = colorFmt;

	if(ColorFormat == ECF_UNKNOWN)
	{
		ColorFormat = CD3D11Helper::getColorFormatFromDXGIFormat(driver->BackBufferFormat);
	}

	if (!RTTexture->createRTTexture(TextureSize, ColorFormat, antialias, quality))
	{
		ASSERT(false);
		return false;
	}

	if (DepthFormat == ECF_UNKNOWN)
	{
		DepthFormat = CD3D11Helper::getColorFormatFromDXGIFormat(driver->DepthStencilFormat);
	}

	if (!DSTexture->createDSTexture(TextureSize, DepthFormat, antialias, quality))
	{
		ASSERT(false);
		return false;
	}

	//create resource view
	if (!(createViews(CD3D11Helper::getDXGIFormatFromColorFormat(ColorFormat), CD3D11Helper::getDXGIFormatFromColorFormat(DepthFormat), antialias > 1)))
	{
		ASSERT(false);
		return false;
	}

	VideoBuilt = true;
	return true;
}

bool CD3D11RenderTarget::createCopyTextures( dimension2du size, ECOLOR_FORMAT format )
{
	if (!RTCopyTexture->createEmptyTexture(size, format))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void CD3D11RenderTarget::releaseVideoTexture()
{
	SAFE_RELEASE_STRICT(DepthView);
	SAFE_RELEASE_STRICT(RTView);

	VideoBuilt = false;
}

bool CD3D11RenderTarget::createViews( DXGI_FORMAT colorFmt, DXGI_FORMAT depthFmt, bool multisample )
{
	HRESULT hr;

	//rt view
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	::memset( &rtvDesc, 0, sizeof( rtvDesc ) );
	rtvDesc.Format = colorFmt;

	rtvDesc.ViewDimension = multisample ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	ID3D11Device* pDevice = driver->pID3DDevice11;

	hr = pDevice->CreateRenderTargetView(RTTexture->getDXTexture(), &rtvDesc, &RTView);
	if(FAILED(hr)) { ASSERT(false); return false; }

	//depth view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	::ZeroMemory( &dsDesc, sizeof( dsDesc ) );
	dsDesc.Format = depthFmt;
	dsDesc.Flags = 0;
	dsDesc.ViewDimension = multisample ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	hr = pDevice->CreateDepthStencilView( DSTexture->getDXTexture(), &dsDesc, &DepthView );

	if(FAILED(hr)) 
	{ 
		ASSERT(false); 
		return false; 
	}
	return true;
}

void CD3D11RenderTarget::onLost()
{
	DSTexture->releaseVideoTexture();
	RTTexture->releaseVideoTexture();

	RTCopyTexture->releaseVideoTexture();

	releaseVideoTexture();
}

void CD3D11RenderTarget::onReset()
{
	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(TextureSize, ColorFormat, DepthFormat, setting.antialias * 2, setting.quality);
	ASSERT(success);

	success = createCopyTextures(TextureSize, ColorFormat);
	ASSERT(success);
}

bool CD3D11RenderTarget::writeToRTTexture()
{
	CD3D11Driver* driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());

	DXGI_FORMAT dxgifmt = CD3D11Helper::getDXGIFormatFromColorFormat(RTTexture->getColorFormat());

	if (RTTexture->getSampleCount() > 1)
	{
		driver->ImmediateContext->ResolveSubresource(RTCopyTexture->getDXTexture(), 0, RTTexture->getDXTexture(), 0, dxgifmt);
	}
	else
	{
		driver->ImmediateContext->CopyResource(RTCopyTexture->getDXTexture(), RTTexture->getDXTexture());
	}

	return true;
}

#endif