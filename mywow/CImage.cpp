#include "stdafx.h"
#include "CImage.h"
#include "CBlit.h"

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size )
	: Data(0), Size(size), Format(format)
{
	BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;

	Pitch = BytesPerPixel * Size.Width;

	Data = new u8[Size.Height * Pitch];

	DeleteData = true;
}

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size, void* data, bool deletaData )
	: Data(0), Size(size), Format(format)
{
	BytesPerPixel = getBitsPerPixelFromFormat(Format) / 8;

	Pitch = BytesPerPixel * Size.Width;

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

u32 CImage::getAlphaMask() const
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
		return 0x1<<15;
	case ECF_R5G6B5:
		return 0x0;
	case ECF_R8G8B8:
		return 0x0;
	case ECF_A8R8G8B8:
		return 0xFF000000;
	default:
		return 0x0;
	}
}

u32 CImage::getRedMask() const
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
		return 0x1F<<10;
	case ECF_R5G6B5:
		return 0x1F<<11;
	case ECF_R8G8B8:
		return 0x00FF0000;
	case ECF_A8R8G8B8:
		return 0x00FF0000;
	default:
		return 0x0;
	}
}

u32 CImage::getGreenMask() const
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
		return 0x1F<<5;
	case ECF_R5G6B5:
		return 0x3F<<5;
	case ECF_R8G8B8:
		return 0x0000FF00;
	case ECF_A8R8G8B8:
		return 0x0000FF00;
	default:
		return 0x0;
	}
}

u32 CImage::getBlueMask() const
{
	switch(Format)
	{
	case ECF_A1R5G5B5:
		return 0x1F;
	case ECF_R5G6B5:
		return 0x1F;
	case ECF_R8G8B8:
		return 0x000000FF;
	case ECF_A8R8G8B8:
		return 0x000000FF;
	default:
		return 0x0;
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

	const u32 bpp=getBitsPerPixelFromFormat(format)/8;
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

	switch(Format)
	{
	case ECF_A8R8G8B8:
		resizeBilinearA8R8G8B8(target, width, height, format);
		break;
	default:
		_ASSERT(false);
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
	bool bRes = CBlit::Blit( target, vector2di(0,0), target->getDimension(),
		this, pos );

	_ASSERT(bRes);
}

void CImage::resizeBilinearA8R8G8B8( void* target, u32 w2, u32 h2, ECOLOR_FORMAT format )
{
	u32 bpp = getBitsPerPixelFromFormat(format) / 8;
	u32* pixels = (u32*)Data;
	int w = (int)Size.Width;
	int h = (int)Size.Height;

	u32 a, b, c, d;
	int x, y, index;
	u32 gray, red, blue, green;
	float x_ratio = ((float)(w-1))/w2 ;
	float y_ratio = ((float)(h-1))/h2 ;
	float x_diff, y_diff;
	int offset = 0 ;
	for (u32 i=0;i<h2;i++) {
		for (u32 j=0;j<w2;j++) {
			x = (int)(x_ratio * j) ;
			y = (int)(y_ratio * i) ;
			x_diff = (x_ratio * j) - x ;
			y_diff = (y_ratio * i) - y ;
			index = y*w+x ;

			// range is 0 to 255 thus bitwise AND with 0xff
			a = pixels[index];
			b = pixels[index+1];
			c = pixels[index+w];
			d = pixels[index+w+1];

			gray = (u32) (((a>>24)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>24)&0xff)*(x_diff)*(1-y_diff) +
				((c>>24)&0xff)*(y_diff)*(1-x_diff)   + ((d>>24)&0xff)*(x_diff*y_diff));

			// blue element
			// Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
			blue = (u32) ((a&0xff)*(1-x_diff)*(1-y_diff) + (b&0xff)*(x_diff)*(1-y_diff) +
				(c&0xff)*(y_diff)*(1-x_diff)   + (d&0xff)*(x_diff*y_diff));

			// green element
			// Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
			green = (u32) (((a>>8)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>8)&0xff)*(x_diff)*(1-y_diff) +
				((c>>8)&0xff)*(y_diff)*(1-x_diff)   + ((d>>8)&0xff)*(x_diff*y_diff));

			// red element
			// Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
			red = (u32) (((a>>16)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>16)&0xff)*(x_diff)*(1-y_diff) +
				((c>>16)&0xff)*(y_diff)*(1-x_diff)   + ((d>>16)&0xff)*(x_diff*y_diff));

			SColor color(gray, red, green, blue);
			CBlit::convert_viaFormat(&color, ECF_A8R8G8B8, 1, (u8*)target + bpp * (i * w2 + j), format);                                
		}
	}

}