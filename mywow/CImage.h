#pragma once

#include "core.h"
#include "IImage.h"

class CImage : public IImage
{
public:
	CImage( ECOLOR_FORMAT format, const dimension2du& size, void* data, bool deletaData );

private:
	CImage( ECOLOR_FORMAT format, const dimension2du& size );
	~CImage();

	friend class CImageLoaderBLP;

public:
	//
	virtual void* lock() { return Data; }
	virtual void unlock() {}
	virtual ECOLOR_FORMAT getColorFormat() const { return Format; }
	virtual const dimension2du& getDimension() const { return Size; }

	//
	virtual u32 getBytesPerPixel() const { return getBytesPerPixelFromFormat(Format); }
	virtual u32 getPitch() const { return Pitch; }
	virtual SColor getPixel( u32 x, u32 y ) const;
	virtual void setPixel( u32 x, u32 y, SColor color );

	//scale”√À´œﬂ–‘
	virtual void copyToScaling( void* target, u32 width, u32 height, ECOLOR_FORMAT format=ECF_A8R8G8B8, u32 pitch=0 );
	virtual void copyToScaling( IImage* target );
	virtual void copyTo( IImage* target, const vector2di& pos = vector2di(0,0) );

private:
	u8*		Data;
	dimension2du Size;
	u32		Pitch;
	ECOLOR_FORMAT	Format;

	bool		DeleteData;
};