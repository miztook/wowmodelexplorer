#pragma once

#include "base.h"
#include "core.h"

class CImage;

struct SBlitJob
{
	void * src;
	void * dst;

	int32_t width;
	int32_t height;

	uint32_t srcPitch;
	uint32_t dstPitch;

	uint32_t srcPixelMul;
	uint32_t dstPixelMul;
};

class CBlit
{
public:
	CBlit() {}

public:
	static bool Blit( CImage* dest, const vector2di& destPos, const dimension2du& destDimension, CImage* src, const vector2di& srcPos = vector2di(0,0) );

	static void convert_viaFormat(const void* sP, ECOLOR_FORMAT sF, int32_t sN, void* dP, ECOLOR_FORMAT dF);

public:
	static void convert1BitTo16Bit(const uint8_t* in, uint16_t* out, int32_t width, int32_t height, int32_t linepad=0, bool flip=false);

	static void convert4BitTo16Bit(const uint8_t* in, uint16_t* out, int32_t width, int32_t height, const int32_t* palette, int32_t linepad=0, bool flip=false);

	static void convert8BitTo16Bit(const uint8_t* in, uint16_t* out, int32_t width, int32_t height, const int32_t* palette, int32_t linepad=0, bool flip=false);

	static void convert16BitTo16Bit(const uint16_t* in, uint16_t* out, int32_t width, int32_t height, int32_t linepad=0, bool flip=false);

	static void convert24BitTo24Bit(const uint8_t* in, uint8_t* out, int32_t width, int32_t height, int32_t linepad=0, bool flip=false, bool changeRB = false);

	static void convert32BitTo32Bit(const int32_t* in, int32_t* out, int32_t width, int32_t height, int32_t linepad, bool flip=false, bool changeRB = false);

	//最近点采样 速度最快
	static void resizeNearest(const void* src, uint32_t w1, uint32_t h1, ECOLOR_FORMAT srcFormat, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT targetFormat);
	static void resizeNearest(const void* src, uint32_t w1, uint32_t h1, uint32_t srcPitch, ECOLOR_FORMAT srcFormat,
		void* target, uint32_t w2, uint32_t h2, uint32_t targetPitch, ECOLOR_FORMAT targetFormat);
	
	//双线性，提高精度，黑边问题
	static void resizeBilinearA8R8G8B8( const void* src, uint32_t w1, uint32_t h1, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT format );
	static void resizeBilinearR8G8B8( const void* src, uint32_t w1, uint32_t h1, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT format );

	//bicubic interpolation，最精确，但仍然有黑边问题
	static void resizeBicubicA8R8G8B8( const void* src, uint32_t w1, uint32_t h1, void* target, uint32_t w2, uint32_t h2, ECOLOR_FORMAT format );


	static void copyA8L8MipMap(const uint8_t* src, uint8_t* tgt,
		uint32_t width, uint32_t height,  uint32_t pitchsrc, uint32_t pitchtgt);
	static void copy16BitMipMap(const uint8_t* src, uint8_t* tgt,
		uint32_t width, uint32_t height,  uint32_t pitchsrc, uint32_t pitchtgt, ECOLOR_FORMAT format);
	static void copy32BitMipMap(const uint8_t* src, uint8_t* tgt,
		uint32_t width, uint32_t height,  uint32_t pitchsrc, uint32_t pitchtgt, ECOLOR_FORMAT format);
	static void copyR8G8B8BitMipMap(const uint8_t* src, uint8_t* tgt,
		uint32_t width, uint32_t height,  uint32_t pitchsrc, uint32_t pitchtgt);

private:
	static void executeBlit_TextureCopy_x_to_x( const SBlitJob * job );
	static void executeBlit_TextureCopy_16_to_32( const SBlitJob * job );
	static void executeBlit_TextureCopy_32_to_16( const SBlitJob * job );
	static void executeBlit_TextureCopy_16_to_24( const SBlitJob * job );
	static void executeBlit_TextureCopy_24_to_16( const SBlitJob * job );
	static void executeBlit_TextureCopy_24_to_32( const SBlitJob * job );
	static void executeBlit_TextureCopy_32_to_24( const SBlitJob * job );

public:
	static void convert_A1R5G5B5toR8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_A1R5G5B5toB8G8R8(const void* sP, int32_t sN, void* dP);
	static void convert_A1R5G5B5toA8R8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_A1R5G5B5toA1R5G5B5(const void* sP, int32_t sN, void* dP);
	static void convert_A1R5G5B5toR5G6B5(const void* sP, int32_t sN, void* dP);

	static void convert_A8R8G8B8toA8B8G8R8(const void* sP, int32_t sN, void* dP);
	static void convert_A8R8G8B8toR8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_A8R8G8B8toB8G8R8(const void* sP, int32_t sN, void* dP);
	static void convert_A8R8G8B8toA8R8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_A8R8G8B8toA1R5G5B5(const void* sP, int32_t sN, void* dP);
	static void convert_A8R8G8B8toR5G6B5(const void* sP, int32_t sN, void* dP);

	static void convert_R8G8B8toR8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_R8G8B8toA8R8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_R8G8B8toA1R5G5B5(const void* sP, int32_t sN, void* dP);
	static void convert_R8G8B8toR5G6B5(const void* sP, int32_t sN, void* dP);

	static void convert_R5G6B5toR5G6B5(const void* sP, int32_t sN, void* dP);
	static void convert_R5G6B5toR8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_R5G6B5toB8G8R8(const void* sP, int32_t sN, void* dP);
	static void convert_R5G6B5toA8R8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_R5G6B5toA1R5G5B5(const void* sP, int32_t sN, void* dP);

	//TGA用B8G8R8
	static void convert_B8G8R8toA8R8G8B8(const void* sP, int32_t sN, void* dP);
	static void convert_B8G8R8A8toA8R8G8B8(const void* sP, int32_t sN, void* dP);
};