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
	virtual bool isValid() const { return RTTSurface != nullptr && DepthSurface != nullptr; }
	virtual ITexture* getRTTexture() const;
	virtual bool writeToRTTexture(); 

	//
	 IDirect3DSurface9*	getRTTSurface() const { return RTTSurface; }
	 IDirect3DSurface9* getRTTDepthSurface() const { return DepthSurface; }

private:
	bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, uint32_t antialias, uint32_t quality);
	void releaseVideoTexture();

private:
	CD3D9Texture*		RTCopyTexture;

	IDirect3DSurface9*		RTTSurface;
	IDirect3DSurface9*		DepthSurface;
};

#endif