#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "SColor.h"

class IImage : public IReferenceCounted<IImage>
{
protected:
	virtual void onRemove() {}
	virtual ~IImage() {}
	
public:
	IImage( ECOLOR_FORMAT format, const dimension2du& size ) : Data(NULL_PTR), Size(size), Format(format)
	{
		Pitch = ROUND_4BYTES(getBytesPerPixel() * Size.Width);
	}
	//
	void* lock() { return Data; }
	void unlock() {}

	ECOLOR_FORMAT getColorFormat() const { return Format; }
	const dimension2du& getDimension() const { return Size; }
	u32 getBytesPerPixel() const { return getBytesPerPixelFromFormat(Format); }
	u32 getPitch() const { return Pitch; }
	const u8* getData() const { return Data; }

	SColor getPixel( u32 x, u32 y ) const;
	void setPixel( u32 x, u32 y, SColor color );

	//scaleÓÃË«ÏßÐÔ
	virtual void copyToScaling( void* target, u32 width, u32 height, ECOLOR_FORMAT format=ECF_A8R8G8B8, u32 pitch=0 ) = 0;
	virtual void copyToScaling( IImage* target ) = 0;
	virtual void copyTo( IImage* target, const vector2di& pos = vector2di(0,0) ) = 0;
	virtual bool checkHasAlpha() const = 0;

protected:
	u8*		Data;
	dimension2du Size;
	u32		Pitch;
	ECOLOR_FORMAT	Format;
};

inline SColor IImage::getPixel( u32 x, u32 y ) const
{
	if (x >= Size.Width || y >= Size.Height)
		return SColor(0);

	switch(Format)
	{
	case ECF_A1R5G5B5:
		return SColor::A1R5G5B5toA8R8G8B8(((u16*)Data)[y*Size.Width + x]);
	case ECF_R5G6B5:
		return SColor::R5G6B5toA8R8G8B8(((u16*)Data)[y*Size.Width + x]);
	case ECF_A8R8G8B8:
		return ((u32*)Data)[y*Size.Width + x];
	case ECF_R8G8B8:
		{
			u8* p = Data+(y*3)*Size.Width + (x*3);
			return SColor(p[0],p[1],p[2]);
		}
	default:{}
	}

	return SColor(0);
}

inline void IImage::setPixel( u32 x, u32 y, SColor color )
{
	if (x >= Size.Width || y >= Size.Height)
		return;

	switch(Format)
	{
	case ECF_A1R5G5B5:
		{
			u16 * dest = (u16*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = SColor::A8R8G8B8toA1R5G5B5( color.color );
		} break;

	case ECF_R5G6B5:
		{
			u16 * dest = (u16*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = SColor::A8R8G8B8toR5G6B5( color.color );
		} break;

	case ECF_R8G8B8:
		{
			u8* dest = Data + ( y * Pitch ) + ( x * 3 );
			dest[0] = (u8)color.getRed();
			dest[1] = (u8)color.getGreen();
			dest[2] = (u8)color.getBlue();
		} break;

	case ECF_A8R8G8B8:
		{
			u32 * dest = (u32*) (Data + ( y * Pitch ) + ( x << 2 ));
			*dest = color.color;
		} break;
	default:{}
	}
}
