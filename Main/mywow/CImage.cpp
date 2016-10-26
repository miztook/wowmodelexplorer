#include "stdafx.h"
#include "CImage.h"
#include "CBlit.h"

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size )
	: IImage(format, size)
{
	Data = new u8[Size.Height * Pitch];

	DeleteData = true;
}

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size, const void* data, bool deletaData )
	: IImage(format, size)
{
	Data = (u8*)data;

	ASSERT(Data);

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
		CBlit::resizeNearest(Data, Size.Width, Size.Height, Pitch, Format, target, width, height, pitch, format);
	}
	else
	{
		switch(Format)
		{
		case ECF_A8R8G8B8:
			CBlit::resizeBilinearA8R8G8B8(Data, Size.Width, Size.Height, target, width, height, format);
			//CBlit::resampleA8R8G8B8(Data, Size.Width, Size.Height, target, width, height, format);
			break;
		default: 
			ASSERT(false);
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
	bool bRes = CBlit::Blit( static_cast<CImage*>(target), vector2di(0,0), target->getDimension(),
		this, pos );

	ASSERT(bRes);
}

bool CImage::checkHasAlpha() const
{
	bool alpha = false;
	for (u32 x = 0; x < Size.Width; ++x)
	{
		for (u32 y = 0; y < Size.Height; ++y)
		{
			SColor color = getPixel(x, y);
			alpha = (color.getAlpha() != 255);
			if (alpha)
				break;
		}
	}
	return alpha;
}
