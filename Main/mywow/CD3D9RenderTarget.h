#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IRenderTarget.h"

class CD3D9Texture;

class CD3D9RenderTarget : public IRenderTarget
{
public:
	CD3D9RenderTarget(const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt);
	~CD3D9RenderTarget();

public:
	virtual bool isValid() const { return RTTSurface != NULL_PTR && DepthSurface != NULL_PTR; }
	virtual ITexture* getRTTexture() const;
	virtual bool writeToRTTexture(); 

	//lost reset
	void onLost();
	void onReset();

	//
	 IDirect3DSurface9*	getRTTSurface() const { return RTTSurface; }
	 IDirect3DSurface9* getRTTDepthSurface() const { return DepthSurface; }

private:
	bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, u32 antialias, u32 quality);
	void releaseVideoTexture();

private:
	CD3D9Texture*		RTCopyTexture;

	IDirect3DSurface9*		RTTSurface;
	IDirect3DSurface9*		DepthSurface;
};

#endif