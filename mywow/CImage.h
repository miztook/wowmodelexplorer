#pragma once

#include "base.h"
#include "core.h"
#include "IImage.h"

class CImage : public IImage
{
public:
	CImage( ECOLOR_FORMAT format, const dimension2du& size );
	CImage( ECOLOR_FORMAT format, const dimension2du& size, void* data, bool deletaData );
	virtual ~CImage();

public:
	//
	virtual void* lock() { return Data; }
	virtual void unlock() {}

	//
	virtual u32 getBitsPerPixel() const { return getBitsPerPixelFromFormat(Format); }
	virtual u32 getBytesPerPixel() const { return BytesPerPixel; }
	virtual u32 getImageDataSizeInBytes() const { return Pitch * Size.Height; }
	virtual u32 getImageDataSizeInPixels() const { return Size.Width * Size.Height; }
	virtual const dimension2du& getDimension() const { return Size; }
	virtual u32 getAlphaMask() const;
	virtual u32 getRedMask() const;
	virtual u32 getGreenMask() const;
	virtual u32 getBlueMask() const;
	virtual u32 getPitch() const { return Pitch; }
	virtual ECOLOR_FORMAT getColorFormat() const { return Format; }
	virtual SColor getPixel( u32 x, u32 y ) const;
	virtual void setPixel( u32 x, u32 y, SColor color );

	virtual void copyToScaling( void* target, u32 width, u32 height, ECOLOR_FORMAT format=ECF_A8R8G8B8, u32 pitch=0 );
	virtual void copyToScaling( IImage* target );
	virtual void copyTo( IImage* target, const vector2di& pos = vector2di(0,0) );

private:
	void resizeBilinearA8R8G8B8(void* target, u32 w2, u32 h2, ECOLOR_FORMAT format);

private:
	u8*		Data;
	dimension2du Size;
	u32 BytesPerPixel;
	u32 Pitch;
	ECOLOR_FORMAT	Format;

	bool		DeleteData;
};