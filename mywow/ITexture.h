#pragma once

#include "core.h"
#include "IResourceCache.h"

class ITexture : public IReferenceCounted<ITexture>, public ILostResetCallback
{
protected:
	virtual void onRemove() 
	{
		releaseVideoTexture();
	}

public:
	ITexture() : VideoBuilt(false) {}
	virtual ~ITexture() { }

public:
	virtual bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format ) = 0;
	virtual bool createEmptyTexture( dimension2du size, ECOLOR_FORMAT format, bool mipmap ) = 0;
	virtual void* lock(bool readOnly = false, u32 mipmapLevel=0) = 0;
	virtual void unlock() = 0;

	virtual dimension2du getSize() const = 0;
	virtual ECOLOR_FORMAT getColorFormat() const = 0;
	virtual bool hasMipMaps() const = 0;
	virtual bool isRenderTarget() const = 0;

	virtual bool isValid() const = 0;

	virtual bool createMipMaps( u32 level = 1 ) = 0;

	//video memory
	virtual bool createVideoTexture() = 0;
	virtual void releaseVideoTexture() = 0;

protected:
	bool VideoBuilt;
};