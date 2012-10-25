#pragma once

#include "IRenderTarget.h"

class CD3D9RenderTarget : public IRenderTarget
{
public:
	CD3D9RenderTarget(dimension2du size, ECOLOR_FORMAT format);
	~CD3D9RenderTarget();

public:
	virtual void* lock(bool readOnly = false);
	virtual void unlock();

	virtual dimension2du getSize() const { return TextureSize; }
	virtual ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }

	virtual bool isValid() const { return DXTexture != NULL; }

	//lost reset
	void onLost();
	void onReset();

	//
	IDirect3DTexture9*  getDXTexture() const { return DXTexture; }
	IDirect3DSurface9*	getRTTSurface() const { return RTTSurface; }
	IDirect3DSurface9* getRTTDepthSurface() const { return DepthSurface; }

private:
	bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format );
	void releaseVideoTexture();

private:
	IDirect3DTexture9*		DXTexture;
	IDirect3DSurface9*		RTTSurface;
	IDirect3DSurface9*		DepthSurface;

	dimension2du	TextureSize;
	ECOLOR_FORMAT	ColorFormat;
};