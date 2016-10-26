#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "IRenderTarget.h"

class CD3D11Texture;

class CD3D11RenderTarget : public IRenderTarget
{
public:
	CD3D11RenderTarget(const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt);
	~CD3D11RenderTarget();

public:
	virtual bool isValid() const;
	virtual ITexture* getRTTexture() const;
	virtual bool writeToRTTexture();

	//lost reset
	void onLost();
	void onReset();

	 ID3D11RenderTargetView* getRenderTargetView() const { return RTView; }
	 ID3D11DepthStencilView* getDepthStencilView() const { return DepthView; }

private:
	bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, u32 antialias, u32 quality );
	bool createCopyTextures( dimension2du size, ECOLOR_FORMAT format );
	void releaseVideoTexture();		//Îªlock,unlock´´½¨buffer

	bool createViews(DXGI_FORMAT colorFmt, DXGI_FORMAT depthFmt, bool multisample);

private:
	//output
	CD3D11Texture*		RTCopyTexture;

	//shader use RT
	CD3D11Texture*		RTTexture;
	CD3D11Texture*		DSTexture;

	ID3D11RenderTargetView*		RTView;
	ID3D11DepthStencilView*		DepthView;
};

#endif