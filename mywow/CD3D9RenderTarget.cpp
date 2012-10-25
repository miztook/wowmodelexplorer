#include "stdafx.h"
#include "CD3D9RenderTarget.h"
#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "mywow.h"

CD3D9RenderTarget::CD3D9RenderTarget( dimension2du size, ECOLOR_FORMAT format )
	: DXTexture(NULL), RTTSurface(NULL), DepthSurface(NULL), ColorFormat(ECF_UNKNOWN)
{
	bool success = createAsRenderTarget(size, format);
	_ASSERT(success);
}

CD3D9RenderTarget::~CD3D9RenderTarget()
{
	releaseVideoTexture();
}

void* CD3D9RenderTarget::lock( bool readOnly /*= false*/ )
{
	_ASSERT(RTTSurface);

	D3DLOCKED_RECT	rect;
	HRESULT hr = RTTSurface->LockRect( &rect, 0, readOnly ? D3DLOCK_READONLY : 0 );
	_ASSERT( SUCCEEDED(hr) );

	return rect.pBits;
}

void CD3D9RenderTarget::unlock()
{
	_ASSERT(RTTSurface);
	RTTSurface->UnlockRect();
}

bool CD3D9RenderTarget::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format )
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
	if (driver->DefaultDepthBuffer)
	{
		D3DSURFACE_DESC desc;
		driver->DefaultDepthBuffer->GetDesc(&desc);
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

	VideoBuilt = true;
	return true;
}

void CD3D9RenderTarget::releaseVideoTexture()
{
	SAFE_RELEASE(DepthSurface);
	SAFE_RELEASE(RTTSurface);
	SAFE_RELEASE(DXTexture);

	VideoBuilt = false;
}

void CD3D9RenderTarget::onLost()
{
	releaseVideoTexture();
}

void CD3D9RenderTarget::onReset()
{
	bool success = createAsRenderTarget(TextureSize, ColorFormat);
	_ASSERT(success);
}