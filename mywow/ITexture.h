#pragma once

#include "core.h"
#include "IResourceCache.h"

class ITexture : public IReferenceCounted<ITexture>
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
	virtual bool createEmptyTexture( dimension2du size, ECOLOR_FORMAT format ) = 0;
	virtual void* lock(bool readOnly = false) = 0;
	virtual void unlock() = 0;

	virtual dimension2du getSize() const = 0;
	virtual ECOLOR_FORMAT getColorFormat() const = 0;
	virtual bool hasMipMaps() const = 0;;

	virtual bool isValid() const = 0;

	//video memory
	virtual bool createVideoTexture() = 0;
	virtual void releaseVideoTexture() = 0;

protected:
	bool VideoBuilt;
};