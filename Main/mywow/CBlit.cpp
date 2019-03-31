#include "stdafx.h"
#include "CBlit.h"
#include "CImage.h"

bool CBlit::Blit( CImage* dest, const vector2di& destPos, const dimension2du& destDimension, CImage* src, const vector2di& srcPos )
{
	if ( !dest || !src ||
		dest->getDimension().Width < destPos.X + destDimension.Width ||
		dest->getDimension().Height < destPos.Y + destDimension.Height ||
		src->getDimension().Width < srcPos.X + destDimension.Width ||
		src->getDimension().Height < srcPos.Y + destDimension.Height )
	{
		return false;
	}

	SBlitJob job;

	job.width = destDimension.Width;
	job.height = destDimension.Height;

	job.srcPitch = src->getPitch();
	job.srcPixelMul = src->getBytesPerPixel();
	job.src = ( (uint8_t*)src->lock() + job.srcPitch * srcPos.Y + job.srcPixelMul * srcPos.X );

	job.dstPitch = dest->getPitch();
	job.dstPixelMul = dest->getBytesPerPixel();
	job.dst = ( (uint8_t*)dest->lock() + job.dstPitch * destPos.Y + job.dstPixelMul * destPos.X );

	ECOLOR_FORMAT srcFormat = src->getColorFormat();
	ECOLOR_FORMAT destFormat = dest->getColorFormat();

	if ( srcFormat == destFormat )
	{
		executeBlit_TextureCopy_x_to_x( &job );
	}
	else
		if ( srcFormat == ECF_A8R8G8B8)
		{
			if ( destFormat == ECF_A1R5G5B5 )
				executeBlit_TextureCopy_32_to_16(&job);
			else if ( destFormat == ECF_R8G8B8 )
				executeBlit_TextureCopy_32_to_24(&job);
			else
			{
				src->unlock();
				dest->unlock();
				return false;
			}
		}
		else
			if ( srcFormat == ECF_A1R5G5B5 )
			{
				if ( destFormat == ECF_R8G8B8 )
					executeBlit_TextureCopy_16_to_24(&job);
				else if ( destFormat == ECF_A8R8G8B8 )
					executeBlit_TextureCopy_16_to_32(&job);
				else
				{
					src->unlock();
					dest->unlock();
					return false;
				}
			}
			else
				if ( srcFormat == ECF_R8G8B8 )
				{
					if ( destFormat == ECF_A1R5G5B5 )
						executeBlit_TextureCopy_24_to_16(&job);
					else if ( destFormat == ECF_A8R8G8B8 )
						executeBlit_TextureCopy_24_to_32(&job);
					else
					{
						src->unlock();
						dest->unlock();
						return false;
					}
				}
				else
				{
					src->unlock();
					dest->unlock();
					return false;
				}

				src->unlock();
				dest->unlock();

				return true;
}

void CBlit::executeBlit_TextureCopy_x_to_x( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	void *dst = (void*) job->dst;

	const uint32_t widthPitch = job->width * job->dstPixelMul;
	for ( int32_t dy = 0; dy != job->height; ++dy )
	{
		memcpy( dst, src, widthPitch );

		src = (void*) ( (uint8_t*) (src) + job->srcPitch );
		dst = (void*) ( (uint8_t*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_16_to_32( const SBlitJob * job )
{
	const uint16_t *src = (uint16_t*) job->src;
	uint32_t *dst = (uint32_t*) job->dst;

	for ( int32_t dy = 0; dy != job->height; ++dy )
	{
		for ( int32_t dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = SColor::A1R5G5B5toA8R8G8B8( src[dx] );
		}

		src = (uint16_t*) ( (uint8_t*) (src) + job->srcPitch );
		dst = (uint32_t*) ( (uint8_t*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_32_to_16( const SBlitJob * job )
{
	const uint32_t *src = static_cast<const uint32_t*>(job->src);
	uint16_t *dst = static_cast<uint16_t*>(job->dst);

	for ( int32_t dy = 0; dy != job->height; ++dy )
	{
		for ( int32_t dx = 0; dx != job->width; ++dx )
		{
			const uint32_t s = src[dx];
			dst[dx] = SColor::A8R8G8B8toA1R5G5B5( s );
		}

		src = (uint32_t*) ( (uint8_t*) (src) + job->srcPitch );
		dst = (uint16_t*) ( (uint8_t*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_16_to_24( const SBlitJob * job )
{
	const uint16_t *src = (uint16_t*) job->src;
	uint8_t *dst = (uint8_t*) job->dst;

	for ( int32_t dy = 0; dy != job->height; ++dy )
	{
		for ( int32_t dx = 0; dx != job->width; ++dx )
		{
			uint32_t colour = SColor::A1R5G5B5toA8R8G8B8( src[dx] );
			uint8_t * writeTo = &dst[dx * 3];
			*writeTo++ = (colour >> 16)& 0xFF;
			*writeTo++ = (colour >> 8) & 0xFF;
			*writeTo++ = colour & 0xFF;
		}

		src = (uint16_t*) ( (uint8_t*) (src) + job->srcPitch );
		dst += job->dstPitch;
	}
}

void CBlit::executeBlit_TextureCopy_24_to_16( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	uint16_t *dst = (uint16_t*) job->dst;

	for ( int32_t dy = 0; dy != job->height; ++dy )
	{
		uint8_t * s = (uint8_t*) src;

		for ( int32_t dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = SColor::RGB16(s[0], s[1], s[2]);
			s += 3;
		}

		src = (void*) ( (uint8_t*) (src) + job->srcPitch );
		dst = (uint16_t*) ( (uint8_t*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_24_to_32( const SBlitJob * job )
{
	void *src = (void*) job->src;
	uint32_t *dst = (uint32_t*) job->dst;

	for ( int32_t dy = 0; dy != job->height; ++dy )
	{
		uint8_t * s = (uint8_t*) src;

		for ( int32_t dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = 0xFF000000 | s[0] << 16 | s[1] << 8 | s[2];
			s += 3;
		}

		src = (void*) ( (uint8_t*) (src) + job->srcPitch );
		dst = (uint32_t*) ( (uint8_t*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_32_to_24( const SBlitJob * job )
{
	const uint32_t * src = (uint32_t*) job->src;
	uint8_t * dst = (uint8_t*) job->dst;

	for ( int32_t dy = 0; dy != job->height; ++dy )
	{
		for ( int32_t dx = 0; dx != job->width; ++dx )
		{
			uint8_t * writeTo = &dst[dx * 3];
			*writeTo++ = (src[dx] >> 16)& 0xFF;
			*writeTo++ = (src[dx] >> 8) & 0xFF;
			*writeTo++ = src[dx] & 0xFF;
		}

		src = (uint32_t*) ( (uint8_t*) (src) + job->srcPitch );
		dst += job->dstPitch ;
	}
}

void CBlit::convert_viaFormat( const void* sP, ECOLOR_FORMAT sF, int32_t sN, void* dP, ECOLOR_FORMAT dF )
{
	switch (sF)
	{
	case ECF_A1R5G5B5:
		switch (dF)
		{
		case ECF_A1R5G5B5:
			convert_A1R5G5B5toA1R5G5B5(sP, sN, dP);
			break;
		case ECF_R5G6B5:
			convert_A1R5G5B5toR5G6B5(sP,sN,dP);
			break;
		case ECF_A8R8G8B8:
			convert_A1R5G5B5toA8R8G8B8(sP, sN, dP);
			break;
		case ECF_R8G8B8:
			convert_A1R5G5B5toR8G8B8(sP, sN, dP);
			break;
		default:
			ASSERT(false);
            break;
		}
		break;
	case ECF_R5G6B5:
		switch (dF)
		{
		case ECF_A1R5G5B5:
			convert_R5G6B5toA1R5G5B5(sP, sN, dP);
			break;
		case ECF_R5G6B5:
			convert_R5G6B5toR5G6B5(sP, sN, dP);
			break;
		case ECF_A8R8G8B8:
			convert_R5G6B5toA8R8G8B8(sP, sN, dP);
			break;
		case ECF_R8G8B8:
			convert_R5G6B5toR8G8B8(sP, sN, dP);
			break;
        default:
            ASSERT(false);
            break;
		}
		break;
	case ECF_A8R8G8B8:
		switch (dF)
		{
		case ECF_A1R5G5B5:
			convert_A8R8G8B8toA1R5G5B5(sP, sN, dP);
			break;
		case ECF_R5G6B5:
			convert_A8R8G8B8toR5G6B5(sP, sN, dP);
			break;
		case ECF_A8R8G8B8:
			convert_A8R8G8B8toA8R8G8B8(sP, sN, dP);
			break;
		case ECF_R8G8B8:
			convert_A8R8G8B8toR8G8B8(sP, sN, dP);
			break;
		default:
			ASSERT(false);
            break;
		}
		break;
	case ECF_R8G8B8:
		switch (dF)
		{
		case ECF_A1R5G5B5:
			convert_R8G8B8toA1R5G5B5(sP, sN, dP);
			break;
		case ECF_R5G6B5:
			convert_R8G8B8toR5G6B5(sP, sN, dP);
			break;
		case ECF_A8R8G8B8:
			convert_R8G8B8toA8R8G8B8(sP, sN, dP);
			break;
		case ECF_R8G8B8:
			convert_R8G8B8toR8G8B8(sP, sN, dP);
			break;
		default:
			ASSERT(false);
            break;
		}
		break;
    default:
        ASSERT(false);
        break;
	}
}

void CBlit::convert_A1R5G5B5toR8G8B8( const void* sP, int32_t sN, void* dP )
{
	uint16_t* sB = (uint16_t*)sP;
	uint8_t * dB = (uint8_t *)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		dB[2] = (uint8_t)((*sB & 0x7c00) >> 7);
		dB[1] = (uint8_t)((*sB & 0x03e0) >> 2);
		dB[0] = (uint8_t)((*sB & 0x1f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CBlit::convert_A1R5G5B5toB8G8R8(const void* sP, int32_t sN, void* dP)
{
	uint16_t* sB = (uint16_t*)sP;
	uint8_t * dB = (uint8_t *)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		dB[0] = (uint8_t)((*sB & 0x7c00) >> 7);
		dB[1] = (uint8_t)((*sB & 0x03e0) >> 2);
		dB[2] = (uint8_t)((*sB & 0x1f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CBlit::convert_A1R5G5B5toA8R8G8B8( const void* sP, int32_t sN, void* dP )
{
	uint16_t* sB = (uint16_t*)sP;
	uint32_t* dB = (uint32_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
		*dB++ = SColor::A1R5G5B5toA8R8G8B8(*sB++);
}

void CBlit::convert_A1R5G5B5toA1R5G5B5( const void* sP, int32_t sN, void* dP )
{
	memcpy(dP, sP, sN * 2);
}

void CBlit::convert_A1R5G5B5toR5G6B5(const void* sP, int32_t sN, void* dP)
{
	uint16_t* sB = (uint16_t*)sP;
	uint16_t* dB = (uint16_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
		*dB++ = SColor::A1R5G5B5toR5G6B5(*sB++);
}

void CBlit::convert_A8R8G8B8toA8B8G8R8( const void* sP, int32_t sN, void* dP )
{
	uint8_t* sB = (uint8_t*)sP;
	uint8_t* dB = (uint8_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 4;
	}
}

void CBlit::convert_A8R8G8B8toR8G8B8( const void* sP, int32_t sN, void* dP )
{
	uint8_t* sB = (uint8_t*)sP;
	uint8_t* dB = (uint8_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_A8R8G8B8toB8G8R8(const void* sP, int32_t sN, void* dP)
{
	uint8_t* sB = (uint8_t*)sP;
	uint8_t* dB = (uint8_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_A8R8G8B8toA8R8G8B8( const void* sP, int32_t sN, void* dP )
{
	memcpy(dP, sP, sN * 4);
}

void CBlit::convert_A8R8G8B8toA1R5G5B5( const void* sP, int32_t sN, void* dP )
{
	uint32_t* sB = (uint32_t*)sP;
	uint16_t* dB = (uint16_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
		*dB++ = SColor::A8R8G8B8toA1R5G5B5(*sB++);
}

void CBlit::convert_A8R8G8B8toR5G6B5(const void* sP, int32_t sN, void* dP)
{
	uint8_t * sB = (uint8_t *)sP;
	uint16_t* dB = (uint16_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		int32_t r = sB[2] >> 3;
		int32_t g = sB[1] >> 2;
		int32_t b = sB[0] >> 3;

		dB[0] = (uint16_t)((r << 11) | (g << 5) | (b));

		sB += 4;
		dB += 1;
	}
}

void CBlit::convert_R8G8B8toR8G8B8( const void* sP, int32_t sN, void* dP )
{
	memcpy(dP, sP, sN * 3);
}

void CBlit::convert_R8G8B8toA8R8G8B8( const void* sP, int32_t sN, void* dP )
{
	uint8_t*  sB = (uint8_t* )sP;
	uint32_t* dB = (uint32_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[0]<<16) | (sB[1]<<8) | sB[2];

		sB += 4;
		++dB;
	}
}

void CBlit::convert_R8G8B8toA1R5G5B5( const void* sP, int32_t sN, void* dP )
{
	uint8_t * sB = (uint8_t *)sP;
	uint16_t* dB = (uint16_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		int32_t r = sB[0] >> 3;
		int32_t g = sB[1] >> 3;
		int32_t b = sB[2] >> 3;

		dB[0] = (uint16_t)((0x8000) | (r << 10) | (g << 5) | (b));

		sB += 3;
		dB += 1;
	}
}

void CBlit::convert_R8G8B8toR5G6B5(const void* sP, int32_t sN, void* dP)
{
	uint8_t * sB = (uint8_t *)sP;
	uint16_t* dB = (uint16_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		int32_t r = sB[0] >> 3;
		int32_t g = sB[1] >> 2;
		int32_t b = sB[2] >> 3;

		dB[0] = (uint16_t)((r << 11) | (g << 5) | (b));

		sB += 3;
		dB += 1;
	}
}

void CBlit::convert_R5G6B5toR5G6B5(const void* sP, int32_t sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void CBlit::convert_R5G6B5toR8G8B8(const void* sP, int32_t sN, void* dP)
{
	uint16_t* sB = (uint16_t*)sP;
	uint8_t * dB = (uint8_t *)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		dB[0] = (uint8_t)((*sB & 0xf800) << 8);
		dB[1] = (uint8_t)((*sB & 0x07e0) << 2);
		dB[2] = (uint8_t)((*sB & 0x001f) << 3);

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_R5G6B5toB8G8R8(const void* sP, int32_t sN, void* dP)
{
	uint16_t* sB = (uint16_t*)sP;
	uint8_t * dB = (uint8_t *)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		dB[2] = (uint8_t)((*sB & 0xf800) << 8);
		dB[1] = (uint8_t)((*sB & 0x07e0) << 2);
		dB[0] = (uint8_t)((*sB & 0x001f) << 3);

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_R5G6B5toA8R8G8B8(const void* sP, int32_t sN, void* dP)
{
	uint16_t* sB = (uint16_t*)sP;
	uint32_t* dB = (uint32_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
		*dB++ = SColor::R5G6B5toA8R8G8B8(*sB++);
}

void CBlit::convert_R5G6B5toA1R5G5B5(const void* sP, int32_t sN, void* dP)
{
	uint16_t* sB = (uint16_t*)sP;
	uint16_t* dB = (uint16_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
		*dB++ = SColor::R5G6B5toA1R5G5B5(*sB++);
}

void CBlit::convert_B8G8R8toA8R8G8B8(const void* sP, int32_t sN, void* dP)
{
	uint8_t*  sB = (uint8_t* )sP;
	uint32_t* dB = (uint32_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[2]<<16) | (sB[1]<<8) | sB[0];

		sB += 3;
		++dB;
	}
}

void CBlit::convert_B8G8R8A8toA8R8G8B8(const void* sP, int32_t sN, void* dP)
{
	uint8_t* sB = (uint8_t*)sP;
	uint8_t* dB = (uint8_t*)dP;

	for (int32_t x = 0; x < sN; ++x)
	{
		dB[0] = sB[3];
		dB[1] = sB[2];
		dB[2] = sB[1];
		dB[3] = sB[0];

		sB += 4;
		dB += 4;
	}
}

void CBlit::convert1BitTo16Bit( const uint8_t* in, uint16_t* out, int32_t width, int32_t height, int32_t linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out)
	{
		ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (int32_t y=0; y<height; ++y)
	{
		int32_t shift = 7;
		if (flip)
			out -= width;

		for (int32_t x=0; x<width; ++x)
		{
			out[x] = *in>>shift & 0x01 ? (uint16_t)0xffff : (uint16_t)0x8000;

			if ((--shift)<0) // 8 pixel done
			{
				shift=7;
				++in;
			}
		}

		if (shift != 7) // width did not fill last byte
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}

void CBlit::convert4BitTo16Bit( const uint8_t* in, uint16_t* out, int32_t width, int32_t height, const int32_t* palette, int32_t linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out || !palette)
	{
		ASSERT(false);
		return;
	}

	if (flip)
		out += width*height;

	for (int32_t y=0; y<height; ++y)
	{
		int32_t shift = 4;
		if (flip)
			out -= width;

		for (int32_t x=0; x<width; ++x)
		{
			out[x] = SColor::X8R8G8B8toA1R5G5B5(palette[(uint8_t)((*in >> shift) & 0xf)]);

			if (shift==0)
			{
				shift = 4;
				++in;
			}
			else
				shift = 0;
		}

		if (shift == 0) // odd width
			++in;

		if (!flip)
			out += width;
		in += linepad;
	}
}

void CBlit::convert8BitTo16Bit( const uint8_t* in, uint16_t* out, int32_t width, int32_t height, const int32_t* palette, int32_t linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out || !palette)
	{
		ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (int32_t y=0; y<height; ++y)
	{
		if (flip)
			out -= width; // one line back
		for (int32_t x=0; x<width; ++x)
		{
			out[x] = SColor::X8R8G8B8toA1R5G5B5(palette[(uint8_t)(*in)]);
			++in;
		}
		if (!flip)
			out += width;
		in += linepad;
	}
}

void CBlit::convert16BitTo16Bit( const uint16_t* in, uint16_t* out, int32_t width, int32_t height, int32_t linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out)
	{
		ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (int32_t y=0; y<height; ++y)
	{
		if (flip)
			out -= width;

		memcpy(out, in, width*sizeof(uint16_t));

		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}

void CBlit::convert24BitTo24Bit( const uint8_t* in, uint8_t* out, int32_t width, int32_t height, int32_t linepad/*=0*/, bool flip/*=false*/, bool changeRB /*= false*/ )
{
	if (!in || !out)
	{
		ASSERT(false);
		return;
	}

	const int32_t lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (int32_t y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth;

		if (changeRB)
		{
			for (int32_t x=0; x<lineWidth; x+=3)
			{
				out[x+0] = in[x+2];
				out[x+1] = in[x+1];
				out[x+2] = in[x+0];
			}
		}
		else
		{
			memcpy(out,in,lineWidth);
		}

		if (!flip)
			out += lineWidth;
		in += lineWidth;
		in += linepad;
	}
}

void CBlit::convert32BitTo32Bit( const int32_t* in, int32_t* out, int32_t width, int32_t height, int32_t linepad, bool flip/*=false*/, bool changeRB /*= false*/ )
{
	if (!in || !out)
	{
		ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (int32_t y=0; y<height; ++y)
	{
		if (flip)
			out -= width;

		if (changeRB)
		{
			uint8_t* pOut = (uint8_t*)out;
			uint8_t* pIn = (uint8_t*)in;
			for (int32_t x=0; x<4*width; x+=4)
			{
				pOut[x] = pIn[x+2];
				pOut[x+1] = pIn[x+1];
				pOut[x+2] = pIn[x];
				pOut[x+3] = pIn[x+3];
			}
		}
		else
		{
			memcpy(out, in, width*sizeof(int32_t));
		}

		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}


void CBlit::resizeBilinearA8R8G8B8( const void* src, uint32_t w1, uint32_t h1, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT format )
{
	uint32_t bpp = getBytesPerPixelFromFormat(format);
	uint32_t* pixels = (uint32_t*)src;
	int w = (int)w1;
	int h = (int)h1;

	uint32_t a, b, c, d;
	int x, y, index;
	uint32_t gray, red, blue, green;
	float x_ratio = ((float)(w-1))/w2 ;
	float y_ratio = ((float)(h-1))/h2 ;
	float x_diff, y_diff;
	for (uint32_t i=0;i<h2;i++) {
		for (uint32_t j=0;j<w2;j++) {
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
			
			gray = (uint32_t) (((a>>24)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>24)&0xff)*(x_diff)*(1-y_diff) +
				((c>>24)&0xff)*(y_diff)*(1-x_diff)   + ((d>>24)&0xff)*(x_diff*y_diff));

			// blue element
			// Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
			blue = (uint32_t) ((a&0xff)*(1-x_diff)*(1-y_diff) + (b&0xff)*(x_diff)*(1-y_diff) +
				(c&0xff)*(y_diff)*(1-x_diff)   + (d&0xff)*(x_diff*y_diff));

			// green element
			// Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
			green = (uint32_t) (((a>>8)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>8)&0xff)*(x_diff)*(1-y_diff) +
				((c>>8)&0xff)*(y_diff)*(1-x_diff)   + ((d>>8)&0xff)*(x_diff*y_diff));

			// red element
			// Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
			red = (uint32_t) (((a>>16)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>16)&0xff)*(x_diff)*(1-y_diff) +
				((c>>16)&0xff)*(y_diff)*(1-x_diff)   + ((d>>16)&0xff)*(x_diff*y_diff));

			SColor color(gray, red, green, blue);
			convert_viaFormat(&color, ECF_A8R8G8B8, 1, (uint8_t*)target + bpp * (i * w2 + j), format);                                
		}
	}
}

void CBlit::resizeBilinearR8G8B8( const void* src, uint32_t w1, uint32_t h1, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT format )
{
	uint32_t bpp = getBytesPerPixelFromFormat(format);
	uint8_t* pixels = (uint8_t*)src;
	int w = (int)w1;
	int h = (int)h1;

	uint8_t a[3], b[3], c[3], d[3];
	int x, y, index;
	uint8_t red, blue, green;
	float x_ratio = ((float)(w-1))/w2 ;
	float y_ratio = ((float)(h-1))/h2 ;
	float x_diff, y_diff;
	for (uint32_t i=0;i<h2;i++) {
		for (uint32_t j=0;j<w2;j++) {
			x = (int)(x_ratio * j) ;
			y = (int)(y_ratio * i) ;
			x_diff = (x_ratio * j) - x ;
			y_diff = (y_ratio * i) - y ;
			index = y*w+x ;

			// range is 0 to 255 thus bitwise AND with 0xff
			a[0] = pixels[index * 3];
			a[1] = pixels[index * 3 + 1];
			a[2] = pixels[index * 3 + 2];
			b[0] = pixels[(index+1) * 3];
			b[1] = pixels[(index+1) * 3 + 1];
			b[2] = pixels[(index+1) * 3 + 2];
			c[0] = pixels[(index+w) * 3];
			c[1] = pixels[(index+w) * 3 + 1];
			c[2] = pixels[(index+w) * 3 + 2];
			d[0] = pixels[(index+w+1) * 3];
			d[1] = pixels[(index+w+1) * 3 + 1];
			d[2] = pixels[(index+w+1) * 3 + 2];

			// blue element
			// Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
			blue = (uint8_t) (a[2]*(1-x_diff)*(1-y_diff) + b[2]*(x_diff)*(1-y_diff) +
				c[2]*(y_diff)*(1-x_diff)  + d[2]*(x_diff*y_diff));

			// green element
			// Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
			green = (uint8_t) (a[1]*(1-x_diff)*(1-y_diff) + b[1]*(x_diff)*(1-y_diff) +
				c[1]*(y_diff)*(1-x_diff)  + d[1]*(x_diff*y_diff));

			// red element
			// Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
			red = (uint8_t) (a[0]*(1-x_diff)*(1-y_diff) + b[0]*(x_diff)*(1-y_diff) +
				c[0]*(y_diff)*(1-x_diff)  + d[0]*(x_diff*y_diff));

			SColor color(red, green, blue);
			convert_viaFormat(&color, ECF_R8G8B8, 1, (uint8_t*)target + bpp * (i * w2 + j), format);                                
		}
	}
}

void CBlit::resizeBicubicA8R8G8B8( const void* src, uint32_t w1, uint32_t h1, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT format )
{
	uint32_t bpp = getBytesPerPixelFromFormat(format);
	uint32_t* pixels = (uint32_t*)src;
	int w = (int)w1;
	int h = (int)h1;

	float xscale = (float)w1/(float)w2;
	float yscale = (float)h1/(float)h2;

	int index;
	float gray, red, blue, green;
	float f_x, f_y, a, b, r1, r2;
	int i_x, i_y, xx, yy;

	for (uint32_t y=0; y<h2; ++y)
	{
		f_y = y * yscale - 0.5f;
		i_y = (int)floor32_(f_y);
		a = f_y - (float)floor32_(f_y);

		for (uint32_t x=0; x<w2; ++x)
		{
			f_x = x * xscale - 0.5f;
			i_x = (int)floor32_(f_x);
			b = f_x - (float)floor32_(f_x);

			//clear
			gray = red = green = blue = 0;

			for (int m=-1; m<3; ++m)
			{
				r1 = KernelBSpline((float)m - a);
				yy = i_y + m;

				if (yy < 0)
					yy = 0;
				if (yy >= (int)h1)
					yy = (int)h1 -1;
				
				for (int n=-1; n<3; ++n)
				{
					r2 = r1 * KernelBSpline(b - (float)n);
					xx = i_x + n;
					if (xx < 0)
						xx = 0;
					if (xx >= (int)w1)
						xx = (int)w1 - 1;

					index = yy*w+xx ;

					// range is 0 to 255 thus bitwise AND with 0xff
					SColor c(pixels[index]);

					gray += c.getAlpha() * r2;
					red += c.getRed() * r2;
					green += c.getGreen() * r2;
					blue += c.getBlue() * r2;
				}
			}

			// set to dest
			SColor color((uint32_t)(uint8_t)gray, (uint32_t)(uint8_t)red, (uint32_t)(uint8_t)green, (uint32_t)(uint8_t)blue);
			convert_viaFormat(&color, ECF_A8R8G8B8, 1, (uint8_t*)target + bpp * (y * w2 + x), format); 
		}
	}
}

void CBlit::resizeNearest( const void* src, uint32_t w1, uint32_t h1, ECOLOR_FORMAT srcFormat, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT targetFormat )
{
	uint32_t bytesPerPixel = getBytesPerPixelFromFormat(srcFormat);
	const uint32_t bpp=getBytesPerPixelFromFormat(targetFormat);

	uint32_t srcPitch = bytesPerPixel * w1;
	uint32_t targetPitch = bpp * w2;

	const float sourceXStep = (float)w1 / (float)w2;
	const float sourceYStep = (float)h1 / (float)h2;
	int32_t yval=0, syval=0;
	float sy = 0.0f;
	for (uint32_t y=0; y<h2; ++y)
	{
		float sx = 0.0f;
		for (uint32_t x=0; x<w2; ++x)
		{
			convert_viaFormat((uint8_t*)src+ syval + (int32_t)sx*bytesPerPixel, srcFormat, 1, ((uint8_t*)target)+ yval + (x*bpp), targetFormat);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=(int32_t)sy*srcPitch;
		yval+=targetPitch;
	}
}

void CBlit::resizeNearest( const void* src, uint32_t w1, uint32_t h1, uint32_t srcPitch, ECOLOR_FORMAT srcFormat, void* target, uint32_t w2, uint32_t h2, uint32_t targetPitch, ECOLOR_FORMAT targetFormat )
{
	uint32_t bytesPerPixel = getBytesPerPixelFromFormat(srcFormat);
	const uint32_t bpp=getBytesPerPixelFromFormat(targetFormat);

	const float sourceXStep = (float)w1 / (float)w2;
	const float sourceYStep = (float)h1 / (float)h2;
	int32_t yval=0, syval=0;
	float sy = 0.0f;
	for (uint32_t y=0; y<h2; ++y)
	{
		float sx = 0.0f;
		for (uint32_t x=0; x<w2; ++x)
		{
			convert_viaFormat((uint8_t*)src+ syval + (int32_t)sx*bytesPerPixel, srcFormat, 1, ((uint8_t*)target)+ yval + (x*bpp), targetFormat);
			sx+=sourceXStep;
		}
		sy+=sourceYStep;
		syval=(int32_t)sy*srcPitch;
		yval+=targetPitch;
	}
}

void CBlit::copyA8L8MipMap( const uint8_t* src, uint8_t* tgt, uint32_t width, uint32_t height, uint32_t pitchsrc, uint32_t pitchtgt )
{
	for (uint32_t y=0; y<height; ++y)
	{
		for (uint32_t x=0; x<width; ++x)
		{
			uint32_t a=0, l = 0;

			for (int32_t dy=0; dy<2; ++dy)
			{
				const int32_t tgy = (y*2)+dy;
				for (int32_t dx=0; dx<2; ++dx)
				{
					const int32_t tgx = (x*2)+dx;

					uint16_t c = *(uint16_t*)(&src[(tgx*2)+(tgy*pitchsrc)]);

					a += (c & 0xff00) >> 8;
					l += (c & 0xff);
				}
			}

			a /= 4;
			l /= 4;

			uint16_t c = a<<8 | l; 
			*(uint16_t*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
	}
}

void CBlit::copy16BitMipMap( const uint8_t* src, uint8_t* tgt, uint32_t width, uint32_t height, uint32_t pitchsrc, uint32_t pitchtgt, ECOLOR_FORMAT format )
{
	ASSERT(format == ECF_A1R5G5B5 || format == ECF_R5G6B5);

	for (uint32_t y=0; y<height; ++y)
	{
		for (uint32_t x=0; x<width; ++x)
		{
			uint32_t a=0, r=0, g=0, b=0;

			for (int32_t dy=0; dy<2; ++dy)
			{
				const int32_t tgy = (y*2)+dy;
				for (int32_t dx=0; dx<2; ++dx)
				{
					const int32_t tgx = (x*2)+dx;

					SColor c;
					if (format == ECF_A1R5G5B5)
						c = SColor::A1R5G5B5toA8R8G8B8(*(uint16_t*)(&src[(tgx*2)+(tgy*pitchsrc)]));
					else
						c = SColor::R5G6B5toA8R8G8B8(*(uint16_t*)(&src[(tgx*2)+(tgy*pitchsrc)]));

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			uint16_t c;
			if (format == ECF_A1R5G5B5)
				c = SColor::RGBA16(r,g,b,a);
			else
				c = SColor::A8R8G8B8toR5G6B5(SColor(a,r,g,b).color);
			*(uint16_t*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
	}
}

void CBlit::copy32BitMipMap( const uint8_t* src, uint8_t* tgt, uint32_t width, uint32_t height, uint32_t pitchsrc, uint32_t pitchtgt, ECOLOR_FORMAT format )
{
	ASSERT(format == ECF_A8R8G8B8);

	for (uint32_t y=0; y<height; ++y)
	{
		for (uint32_t x=0; x<width; ++x)
		{
			uint32_t a=0, r=0, g=0, b=0;
			SColor c;

			for (int32_t dy=0; dy<2; ++dy)
			{
				const int32_t tgy = (y*2)+dy;
				for (int32_t dx=0; dx<2; ++dx)
				{
					const int32_t tgx = (x*2)+dx;

					c = *(uint32_t*)(&src[(tgx*4)+(tgy*pitchsrc)]);

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			if (format == ECF_A8R8G8B8)
				c.set(a, r, g, b);
			else
				c.set(a, b, g, r);

			*(uint32_t*)(&tgt[(x*4)+(y*pitchtgt)]) = c.color;
		}
	}
}

void CBlit::copyR8G8B8BitMipMap( const uint8_t* src, uint8_t* tgt, uint32_t width, uint32_t height, uint32_t pitchsrc, uint32_t pitchtgt )
{
	for (uint32_t y=0; y<height; ++y)
	{
		for (uint32_t x=0; x<width; ++x)
		{
			uint32_t r=0, g=0, b=0;

			for (int32_t dy=0; dy<2; ++dy)
			{
				const int32_t tgy = (y*2)+dy;
				for (int32_t dx=0; dx<2; ++dx)
				{
					const int32_t tgx = (x*2)+dx;

					const uint8_t* s = &src[(tgx*3)+(tgy*pitchsrc)];

					r += s[0];
					g += s[1];
					b += s[2];
				}
			}

			r /= 4;
			g /= 4;
			b /= 4;

			uint8_t* t = &tgt[(x*3)+(y*pitchtgt)];
			t[0] = r;
			t[1] = g;
			t[2] = b;
		}
	}
}