#include "stdafx.h"
#include "CImage.h"
#include "CBlit.h"

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size )
	: IImage(format, size)
{
	Data = new uint8_t[Size.Height * Pitch];

	DeleteData = true;
}

CImage::CImage( ECOLOR_FORMAT format, const dimension2du& size, const void* data, bool deletaData )
	: IImage(format, size)
{
	Data = (uint8_t*)data;

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


void CImage::copyToScaling( void* target, uint32_t width, uint32_t height, ECOLOR_FORMAT format/*=ECF_A8R8G8B8*/, uint32_t pitch/*=0 */ )
{
	if (!target || !width || !height)
		return;

	const uint32_t bpp=getBytesPerPixelFromFormat(format);
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
			uint8_t* tgtpos = (uint8_t*) target;
			uint8_t* srcpos = Data;
			const uint32_t bwidth = width*bpp;
			const uint32_t rest = pitch-bwidth;
			for (uint32_t y=0; y<height; ++y)
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

	const dimension2d<uint32_t>& targetSize = target->getDimension();

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
	for (uint32_t x = 0; x < Size.Width; ++x)
	{
		for (uint32_t y = 0; y < Size.Height; ++y)
		{
			SColor color = getPixel(x, y);
			alpha = (color.getAlpha() != 255);
			if (alpha)
				break;
		}
	}
	return alpha;
}
