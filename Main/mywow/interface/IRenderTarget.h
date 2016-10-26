#pragma once

#include "core.h"
#include "ILostResetCallback.h"

class ITexture;

class IRenderTarget : public ILostResetCallback
{
public:
	IRenderTarget(ECOLOR_FORMAT colorfmt, ECOLOR_FORMAT depthFmt) : ColorFormat(colorfmt), DepthFormat(depthFmt), VideoBuilt(false) { }
	virtual ~IRenderTarget() { }

public:
	const dimension2du& getSize() const { return TextureSize; }
	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }
	ECOLOR_FORMAT getDepthFormat() const { return DepthFormat; }

	virtual bool isValid() const = 0;
	virtual ITexture* getRTTexture() const = 0;
	virtual bool writeToRTTexture() = 0; 

protected:
	dimension2du	TextureSize;
	ECOLOR_FORMAT	ColorFormat;
	ECOLOR_FORMAT	DepthFormat;
	bool VideoBuilt;
};