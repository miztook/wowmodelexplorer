#include "stdafx.h"
#include "CImage.h"
#include "CBlit.h"

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size )
	: Data(0), Size(size), Format(format)
{
	Pitch = getBytesPerPixel() * Size.Width;

	Data = new u8[Size.Height * Pitch];

	DeleteData = true;
}

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size, void* data, bool deletaData )
	: Data(0), Size(size), Format(format)
{
	Pitch = getBytesPerPixel() * Size.Width;

	Data = (u8*)data;

	_ASSERT(Data);

	DeleteData = deletaData;
}


CImage::~CImage()
{
	if ( Data && DeleteData )
	{
		delete[] Data;
		Data = 0;
	}
}

SColor CImage::getPixel( u32 x, u32 y ) const
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
			return SColor(255,p[0],p[1],p[2]);
		}
	}

	return SColor(0);
}

void CImage::setPixel( u32 x, u32 y, SColor color )
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
	}
}

void CImage::copyToScaling( void* target, u32 width, u32 height, ECOLOR_FORMAT format/*=ECF_A8R8G8B8*/, u32 pitch/*=0 */ )
{
	if (!target || !width || !height)
		return;

	const u32 bpp=getBytesPerPixelFromFormat(format);
	if (0==pitch)
		pitch = width*bpp;

	if (Format==format && Size.Width==width && Size.Height==height)
	{
		if (pitch==Pitch)
		{
			memcpy(target, Data, height*pitch);
		}
		else
		{
			u8* tgtpos = (u8*) target;
			u8* srcpos = Data;
			const u32 bwidth = width*bpp;
			const u32 rest = pitch-bwidth;
			for (u32 y=0; y<height; ++y)
			{
				// copy scanline
				memcpy(tgtpos, srcpos, bwidth);
				// clear pitch
				memset(tgtpos+bwidth, 0, rest);
				tgtpos += pitch;
				srcpos += Pitch;
			}
		}
		return;
	}

	if (Size.Width > width || Size.Height > height)
	{
		CBlit::shrinkImage(Data, Size.Width, Size.Height, Pitch, Format, target, width, height, pitch, format);
	}
	else
	{
		switch(Format)
		{
		case ECF_A8R8G8B8:
			CBlit::resizeBilinearA8R8G8B8(Data, Size.Width, Size.Height, target, width, height, format);
			break;
		default: 
			_ASSERT(false);
			break;
		}
	}
}

void CImage::copyToScaling( IImage* target )
{
	if (!target)
		return;

	const dimension2d<u32>& targetSize = target->getDimension();

	if (targetSize==Size)
	{
		copyTo(target);
		return;
	}

	copyToScaling(target->lock(), targetSize.Width, targetSize.Height, target->getColorFormat());
	target->unlock();
}

void CImage::copyTo( IImage* target, const vector2di& pos /*= core::vector2di(0,0) */ )
{
	bool bRes = CBlit::Blit( (CImage*)target, vector2di(0,0), target->getDimension(),
		this, pos );

	_ASSERT(bRes);
}
