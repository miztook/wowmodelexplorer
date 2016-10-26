#pragma once

#include "base.h"
#include "core.h"

class CImage;

struct SBlitJob
{
	void * src;
	void * dst;

	s32 width;
	s32 height;

	u32 srcPitch;
	u32 dstPitch;

	u32 srcPixelMul;
	u32 dstPixelMul;
};

class CBlit
{
public:
	CBlit() {}

public:
	static bool Blit( CImage* dest, const vector2di& destPos, const dimension2du& destDimension, CImage* src, const vector2di& srcPos = vector2di(0,0) );

	static void convert_viaFormat(const void* sP, ECOLOR_FORMAT sF, s32 sN, void* dP, ECOLOR_FORMAT dF);

public:
	static void convert1BitTo16Bit(const u8* in, u16* out, s32 width, s32 height, s32 linepad=0, bool flip=false);

	static void convert4BitTo16Bit(const u8* in, u16* out, s32 width, s32 height, const s32* palette, s32 linepad=0, bool flip=false);

	static void convert8BitTo16Bit(const u8* in, u16* out, s32 width, s32 height, const s32* palette, s32 linepad=0, bool flip=false);

	static void convert16BitTo16Bit(const u16* in, u16* out, s32 width, s32 height, s32 linepad=0, bool flip=false);

	static void convert24BitTo24Bit(const u8* in, u8* out, s32 width, s32 height, s32 linepad=0, bool flip=false, bool changeRB = false);

	static void convert32BitTo32Bit(const s32* in, s32* out, s32 width, s32 height, s32 linepad, bool flip=false, bool changeRB = false);

	//最近点采样 速度最快
	static void resizeNearest(const void* src, u32 w1, u32 h1, ECOLOR_FORMAT srcFormat, void* target, u32 w2, u32 h2, ECOLOR_FORMAT targetFormat);
	static void resizeNearest(const void* src, u32 w1, u32 h1, u32 srcPitch, ECOLOR_FORMAT srcFormat,
		void* target, u32 w2, u32 h2, u32 targetPitch, ECOLOR_FORMAT targetFormat);
	
	//双线性，提高精度，黑边问题
	static void resizeBilinearA8R8G8B8( const void* src, u32 w1, u32 h1, void* target, u32 w2, u32 h2, ECOLOR_FORMAT format );
	static void resizeBilinearR8G8B8( const void* src, u32 w1, u32 h1, void* target, u32 w2, u32 h2, ECOLOR_FORMAT format );

	//bicubic interpolation，最精确，但仍然有黑边问题
	static void resizeBicubicA8R8G8B8( const void* src, u32 w1, u32 h1, void* target, u32 w2, u32 h2, ECOLOR_FORMAT format );


	static void copyA8L8MipMap(const u8* src, u8* tgt,
		u32 width, u32 height,  u32 pitchsrc, u32 pitchtgt);
	static void copy16BitMipMap(const u8* src, u8* tgt,
		u32 width, u32 height,  u32 pitchsrc, u32 pitchtgt, ECOLOR_FORMAT format);
	static void copy32BitMipMap(const u8* src, u8* tgt,
		u32 width, u32 height,  u32 pitchsrc, u32 pitchtgt, ECOLOR_FORMAT format);
	static void copyR8G8B8BitMipMap(const u8* src, u8* tgt,
		u32 width, u32 height,  u32 pitchsrc, u32 pitchtgt);

private:
	static void executeBlit_TextureCopy_x_to_x( const SBlitJob * job );
	static void executeBlit_TextureCopy_16_to_32( const SBlitJob * job );
	static void executeBlit_TextureCopy_32_to_16( const SBlitJob * job );
	static void executeBlit_TextureCopy_16_to_24( const SBlitJob * job );
	static void executeBlit_TextureCopy_24_to_16( const SBlitJob * job );
	static void executeBlit_TextureCopy_24_to_32( const SBlitJob * job );
	static void executeBlit_TextureCopy_32_to_24( const SBlitJob * job );

public:
	static void convert_A1R5G5B5toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A1R5G5B5toB8G8R8(const void* sP, s32 sN, void* dP);
	static void convert_A1R5G5B5toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A1R5G5B5toA1R5G5B5(const void* sP, s32 sN, void* dP);
	static void convert_A1R5G5B5toR5G6B5(const void* sP, s32 sN, void* dP);

	static void convert_A8R8G8B8toA8B8G8R8(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toB8G8R8(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP);
	static void convert_A8R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP);

	static void convert_R8G8B8toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R8G8B8toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R8G8B8toA1R5G5B5(const void* sP, s32 sN, void* dP);
	static void convert_R8G8B8toR5G6B5(const void* sP, s32 sN, void* dP);

	static void convert_R5G6B5toR5G6B5(const void* sP, s32 sN, void* dP);
	static void convert_R5G6B5toR8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R5G6B5toB8G8R8(const void* sP, s32 sN, void* dP);
	static void convert_R5G6B5toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_R5G6B5toA1R5G5B5(const void* sP, s32 sN, void* dP);

	//TGA用B8G8R8
	static void convert_B8G8R8toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_B8G8R8A8toA8R8G8B8(const void* sP, s32 sN, void* dP);
};