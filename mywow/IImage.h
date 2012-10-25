#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "SColor.h"

class IImage : public IReferenceCounted<IImage>
{
private:
	virtual void onRemove() {}

public:
	virtual ~IImage() {}
	
	//
	virtual void* lock()  = 0;
	virtual void unlock() = 0;

	virtual ECOLOR_FORMAT getColorFormat() const= 0;
	virtual const dimension2du& getDimension() const = 0;
	virtual u32 getBytesPerPixel() const  = 0;
	virtual u32 getPitch() const = 0;
	virtual SColor getPixel( u32 x, u32 y ) const = 0;
	virtual void setPixel( u32 x, u32 y, SColor color ) = 0;

	//scale”√À´œﬂ–‘
	virtual void copyToScaling( void* target, u32 width, u32 height, ECOLOR_FORMAT format=ECF_A8R8G8B8, u32 pitch=0 ) = 0;
	virtual void copyToScaling( IImage* target ) = 0;
	virtual void copyTo( IImage* target, const vector2di& pos = vector2di(0,0) ) = 0;
};

