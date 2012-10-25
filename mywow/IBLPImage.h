#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "SColor.h"

class MPQFile;

class IBLPImage : public IReferenceCounted<IBLPImage>
{
private:
	virtual void onRemove() {}

public:
	virtual ~IBLPImage() {}

public:
	virtual bool loadFile(MPQFile* file) = 0;
	virtual ECOLOR_FORMAT getColorFormat() const = 0;
	virtual dimension2du getDimension() const = 0;
	virtual void* getMipmapData(u32 level) const = 0;  
	virtual bool copyMipmapData(u32 level, void* dest, u32 pitch, u32 width, u32 height) = 0;
};
