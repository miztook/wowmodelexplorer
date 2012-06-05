#include "stdafx.h"
#include "CBlit.h"

bool CBlit::Blit( IImage* dest, const vector2di& destPos, const dimension2du& destDimension, IImage* src, const vector2di& srcPos )
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
	job.srcPixelMul = src->getBitsPerPixel();
	job.src = ( (u8*)src->lock() + job.srcPitch * srcPos.Y + job.srcPixelMul * srcPos.X );

	job.dstPitch = dest->getPitch();
	job.dstPixelMul = dest->getBitsPerPixel();
	job.dst = ( (u8*)dest->lock() + job.dstPitch * destPos.Y + job.dstPixelMul * destPos.X );

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
				if ( destFormat = ECF_R8G8B8 )
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

	const u32 widthPitch = job->width * job->dstPixelMul;
	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		memcpy( dst, src, widthPitch );

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (void*) ( (u8*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_16_to_32( const SBlitJob * job )
{
	const u16 *src = (u16*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = SColor::A1R5G5B5toA8R8G8B8( src[dx] );
		}

		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_32_to_16( const SBlitJob * job )
{
	const u32 *src = static_cast<const u32*>(job->src);
	u16 *dst = static_cast<u16*>(job->dst);

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			const u32 s = src[dx];
			dst[dx] = SColor::A8R8G8B8toA1R5G5B5( s );
		}

		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_16_to_24( const SBlitJob * job )
{
	const u16 *src = (u16*) job->src;
	u8 *dst = (u8*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			u32 colour = SColor::A1R5G5B5toA8R8G8B8( src[dx] );
			u8 * writeTo = &dst[dx * 3];
			*writeTo++ = (colour >> 16)& 0xFF;
			*writeTo++ = (colour >> 8) & 0xFF;
			*writeTo++ = colour & 0xFF;
		}

		src = (u16*) ( (u8*) (src) + job->srcPitch );
		dst += job->dstPitch;
	}
}

void CBlit::executeBlit_TextureCopy_24_to_16( const SBlitJob * job )
{
	const void *src = (void*) job->src;
	u16 *dst = (u16*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = SColor::RGB16(s[0], s[1], s[2]);
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u16*) ( (u8*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_24_to_32( const SBlitJob * job )
{
	void *src = (void*) job->src;
	u32 *dst = (u32*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		u8 * s = (u8*) src;

		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			dst[dx] = 0xFF000000 | s[0] << 16 | s[1] << 8 | s[2];
			s += 3;
		}

		src = (void*) ( (u8*) (src) + job->srcPitch );
		dst = (u32*) ( (u8*) (dst) + job->dstPitch );
	}
}

void CBlit::executeBlit_TextureCopy_32_to_24( const SBlitJob * job )
{
	const u32 * src = (u32*) job->src;
	u8 * dst = (u8*) job->dst;

	for ( s32 dy = 0; dy != job->height; ++dy )
	{
		for ( s32 dx = 0; dx != job->width; ++dx )
		{
			u8 * writeTo = &dst[dx * 3];
			*writeTo++ = (src[dx] >> 16)& 0xFF;
			*writeTo++ = (src[dx] >> 8) & 0xFF;
			*writeTo++ = src[dx] & 0xFF;
		}

		src = (u32*) ( (u8*) (src) + job->srcPitch );
		dst += job->dstPitch ;
	}
}

void CBlit::convert_viaFormat( const void* sP, ECOLOR_FORMAT sF, s32 sN, void* dP, ECOLOR_FORMAT dF )
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
		}
		break;
	}
}

void CBlit::convert_A1R5G5B5toR8G8B8( const void* sP, s32 sN, void* dP )
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[2] = (u8)((*sB & 0x7c00) >> 7);
		dB[1] = (u8)((*sB & 0x03e0) >> 2);
		dB[0] = (u8)((*sB & 0x1f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CBlit::convert_A1R5G5B5toB8G8R8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[0] = (u8)((*sB & 0x7c00) >> 7);
		dB[1] = (u8)((*sB & 0x03e0) >> 2);
		dB[2] = (u8)((*sB & 0x1f) << 3);

		sB += 1;
		dB += 3;
	}
}

void CBlit::convert_A1R5G5B5toA8R8G8B8( const void* sP, s32 sN, void* dP )
{
	u16* sB = (u16*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = SColor::A1R5G5B5toA8R8G8B8(*sB++);
}

void CBlit::convert_A1R5G5B5toA1R5G5B5( const void* sP, s32 sN, void* dP )
{
	memcpy(dP, sP, sN * 2);
}

void CBlit::convert_A1R5G5B5toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = SColor::A1R5G5B5toR5G6B5(*sB++);
}

void CBlit::convert_A8R8G8B8toR8G8B8( const void* sP, s32 sN, void* dP )
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_A8R8G8B8toB8G8R8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		// sB[3] is alpha
		dB[0] = sB[0];
		dB[1] = sB[1];
		dB[2] = sB[2];

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_A8R8G8B8toA8R8G8B8( const void* sP, s32 sN, void* dP )
{
	memcpy(dP, sP, sN * 4);
}

void CBlit::convert_A8R8G8B8toA1R5G5B5( const void* sP, s32 sN, void* dP )
{
	u32* sB = (u32*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = SColor::A8R8G8B8toA1R5G5B5(*sB++);
}

void CBlit::convert_A8R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		s32 r = sB[2] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[0] >> 3;

		dB[0] = (u16)((r << 11) | (g << 5) | (b));

		sB += 4;
		dB += 1;
	}
}

void CBlit::convert_R8G8B8toR8G8B8( const void* sP, s32 sN, void* dP )
{
	memcpy(dP, sP, sN * 3);
}

void CBlit::convert_R8G8B8toA8R8G8B8( const void* sP, s32 sN, void* dP )
{
	u8*  sB = (u8* )sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[0]<<16) | (sB[1]<<8) | sB[2];

		sB += 4;
		++dB;
	}
}

void CBlit::convert_R8G8B8toA1R5G5B5( const void* sP, s32 sN, void* dP )
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 3;
		s32 b = sB[2] >> 3;

		dB[0] = (u16)((0x8000) | (r << 10) | (g << 5) | (b));

		sB += 3;
		dB += 1;
	}
}

void CBlit::convert_R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP)
{
	u8 * sB = (u8 *)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		s32 r = sB[0] >> 3;
		s32 g = sB[1] >> 2;
		s32 b = sB[2] >> 3;

		dB[0] = (u16)((r << 11) | (g << 5) | (b));

		sB += 3;
		dB += 1;
	}
}

void CBlit::convert_R5G6B5toR5G6B5(const void* sP, s32 sN, void* dP)
{
	memcpy(dP, sP, sN * 2);
}

void CBlit::convert_R5G6B5toR8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[0] = (u8)((*sB & 0xf800) << 8);
		dB[1] = (u8)((*sB & 0x07e0) << 2);
		dB[2] = (u8)((*sB & 0x001f) << 3);

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_R5G6B5toB8G8R8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u8 * dB = (u8 *)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[2] = (u8)((*sB & 0xf800) << 8);
		dB[1] = (u8)((*sB & 0x07e0) << 2);
		dB[0] = (u8)((*sB & 0x001f) << 3);

		sB += 4;
		dB += 3;
	}
}

void CBlit::convert_R5G6B5toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = SColor::R5G6B5toA8R8G8B8(*sB++);
}

void CBlit::convert_R5G6B5toA1R5G5B5(const void* sP, s32 sN, void* dP)
{
	u16* sB = (u16*)sP;
	u16* dB = (u16*)dP;

	for (s32 x = 0; x < sN; ++x)
		*dB++ = SColor::R5G6B5toA1R5G5B5(*sB++);
}

void CBlit::convert_B8G8R8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u8*  sB = (u8* )sP;
	u32* dB = (u32*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		*dB = 0xff000000 | (sB[2]<<16) | (sB[1]<<8) | sB[0];

		sB += 3;
		++dB;
	}
}

void CBlit::convert_B8G8R8A8toA8R8G8B8(const void* sP, s32 sN, void* dP)
{
	u8* sB = (u8*)sP;
	u8* dB = (u8*)dP;

	for (s32 x = 0; x < sN; ++x)
	{
		dB[0] = sB[3];
		dB[1] = sB[2];
		dB[2] = sB[1];
		dB[3] = sB[0];

		sB += 4;
		dB += 4;
	}
}

void CBlit::convert1BitTo16Bit( const u8* in, s16* out, s32 width, s32 height, s32 linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out)
	{
		_ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		s32 shift = 7;
		if (flip)
			out -= width;

		for (s32 x=0; x<width; ++x)
		{
			out[x] = *in>>shift & 0x01 ? (s16)0xffff : (s16)0x8000;

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

void CBlit::convert4BitTo16Bit( const u8* in, s16* out, s32 width, s32 height, const s32* palette, s32 linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out || !palette)
	{
		_ASSERT(false);
		return;
	}

	if (flip)
		out += width*height;

	for (s32 y=0; y<height; ++y)
	{
		s32 shift = 4;
		if (flip)
			out -= width;

		for (s32 x=0; x<width; ++x)
		{
			out[x] = SColor::X8R8G8B8toA1R5G5B5(palette[(u8)((*in >> shift) & 0xf)]);

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

void CBlit::convert8BitTo16Bit( const u8* in, s16* out, s32 width, s32 height, const s32* palette, s32 linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out || !palette)
	{
		_ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width; // one line back
		for (s32 x=0; x<width; ++x)
		{
			out[x] = SColor::X8R8G8B8toA1R5G5B5(palette[(u8)(*in)]);
			++in;
		}
		if (!flip)
			out += width;
		in += linepad;
	}
}

void CBlit::convert16BitTo16Bit( const s16* in, s16* out, s32 width, s32 height, s32 linepad/*=0*/, bool flip/*=false*/ )
{
	if (!in || !out)
	{
		_ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;

		memcpy(out, in, width*sizeof(s16));

		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}

void CBlit::convert24BitTo24Bit( const u8* in, u8* out, s32 width, s32 height, s32 linepad/*=0*/, bool flip/*=false*/, bool bgr/*=false*/ )
{
	if (!in || !out)
	{
		_ASSERT(false);
		return;
	}

	const s32 lineWidth = 3 * width;
	if (flip)
		out += lineWidth * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= lineWidth;
		if (bgr)
		{
			for (s32 x=0; x<lineWidth; x+=3)
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

void CBlit::convert32BitTo32Bit( const s32* in, s32* out, s32 width, s32 height, s32 linepad, bool flip/*=false*/ )
{
	if (!in || !out)
	{
		_ASSERT(false);
		return;
	}

	if (flip)
		out += width * height;

	for (s32 y=0; y<height; ++y)
	{
		if (flip)
			out -= width;

		memcpy(out, in, width*sizeof(s32));

		if (!flip)
			out += width;
		in += width;
		in += linepad;
	}
}