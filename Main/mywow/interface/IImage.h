#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "SColor.h"

class IImage : public IReferenceCounted<IImage>
{
protected:
	virtual ~IImage() {}
	
public:
	IImage( ECOLOR_FORMAT format, const dimension2du& size ) : Data(nullptr), Size(size), Format(format)
	{
		Pitch = ROUND_4BYTES(getBytesPerPixel() * Size.Width);
	}
	//
	void* lock() { return Data; }
	void unlock() {}

	ECOLOR_FORMAT getColorFormat() const { return Format; }
	const dimension2du& getDimension() const { return Size; }
	uint32_t getBytesPerPixel() const { return getBytesPerPixelFromFormat(Format); }
	uint32_t getPitch() const { return Pitch; }
	const uint8_t* getData() const { return Data; }

	SColor getPixel( uint32_t x, uint32_t y ) const;
	void setPixel( uint32_t x, uint32_t y, SColor color );

	//scaleÓÃË«ÏßÐÔ
	virtual void copyToScaling( void* target, uint32_t width, uint32_t height, ECOLOR_FORMAT format=ECF_A8R8G8B8, uint32_t pitch=0 ) = 0;
	virtual void copyToScaling( IImage* target ) = 0;
	virtual void copyTo( IImage* target, const vector2di& pos = vector2di(0,0) ) = 0;
	virtual bool checkHasAlpha() const = 0;

protected:
	uint8_t*		Data;
	dimension2du Size;
	uint32_t		Pitch;
	ECOLOR_FORMAT	Format;
};

inline SColor IImage::getPixel( uint32_t x, uint32_t y ) const
{
	if (x >= Size.Width || y >= Size.Height)
		return SColor(0);

	switch(Format)
	{
	case ECF_A1R5G5B5:
		return SColor::A1R5G5B5toA8R8G8B8(((uint16_t*)Data)[y*Size.Width + x]);
	case ECF_R5G6B5:
		return SColor::R5G6B5toA8R8G8B8(((uint16_t*)Data)[y*Size.Width + x]);
	case ECF_A8R8G8B8:
		return ((uint32_t*)Data)[y*Size.Width + x];
	case ECF_R8G8B8:
		{
			uint8_t* p = Data+(y*3)*Size.Width + (x*3);
			return SColor(p[0],p[1],p[2]);
		}
	default:{}
	}

	return SColor(0);
}

inline void IImage::setPixel( uint32_t x, uint32_t y, SColor color )
{
	if (x >= Size.Width || y >= Size.Height)
		return;

	switch(Format)
	{
	case ECF_A1R5G5B5:
		{
			uint16_t * dest = (uint16_t*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = SColor::A8R8G8B8toA1R5G5B5( color.color );
		} break;

	case ECF_R5G6B5:
		{
			uint16_t * dest = (uint16_t*) (Data + ( y * Pitch ) + ( x << 1 ));
			*dest = SColor::A8R8G8B8toR5G6B5( color.color );
		} break;

	case ECF_R8G8B8:
		{
			uint8_t* dest = Data + ( y * Pitch ) + ( x * 3 );
			dest[0] = (uint8_t)color.getRed();
			dest[1] = (uint8_t)color.getGreen();
			dest[2] = (uint8_t)color.getBlue();
		} break;

	case ECF_A8R8G8B8:
		{
			uint32_t * dest = (uint32_t*) (Data + ( y * Pitch ) + ( x << 2 ));
			*dest = color.color;
		} break;
	default:{}
	}
}
