#pragma once

#include "core.h"

class ITexture;
class IImage;

//手工创建的texture, 或创建render target
class IManualTextureServices : public ILostResetCallback
{
public:
	virtual ~IManualTextureServices() {}

public:
	virtual ITexture* getManualTexture(const c8* name) = 0;
	virtual ITexture* addTexture( const c8* name , IImage* img) = 0;
	virtual void removeTexture(const c8* name) = 0;

	virtual ITexture* addRenderTarget( dimension2du size, ECOLOR_FORMAT format ) = 0;
	virtual void removeRenderTarget( ITexture* texture ) = 0;

	virtual ITexture* createTextureFromImage(IImage* image, bool mipmap) = 0;
	virtual ITexture* createEmptyTexture(dimension2du size, ECOLOR_FORMAT format, bool mipmap) = 0;
};