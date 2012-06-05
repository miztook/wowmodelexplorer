#pragma once

#include "base.h"
#include "core.h"
#include "IImage.h"

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
	static bool Blit( IImage* dest, const vector2di& destPos, const dimension2du& destDimension, IImage* src, const vector2di& srcPos = vector2di(0,0) );

	static void convert_viaFormat(const void* sP, ECOLOR_FORMAT sF, s32 sN, void* dP, ECOLOR_FORMAT dF);

public:
	static void convert1BitTo16Bit(const u8* in, s16* out, s32 width, s32 height, s32 linepad=0, bool flip=false);

	static void convert4BitTo16Bit(const u8* in, s16* out, s32 width, s32 height, const s32* palette, s32 linepad=0, bool flip=false);

	static void convert8BitTo16Bit(const u8* in, s16* out, s32 width, s32 height, const s32* palette, s32 linepad=0, bool flip=false);

	static void convert16BitTo16Bit(const s16* in, s16* out, s32 width, s32 height, s32 linepad=0, bool flip=false);

	static void convert24BitTo24Bit(const u8* in, u8* out, s32 width, s32 height, s32 linepad=0, bool flip=false, bool bgr=false);

	static void convert32BitTo32Bit(const s32* in, s32* out, s32 width, s32 height, s32 linepad, bool flip=false);

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

	//TGA”√B8G8R8
	static void convert_B8G8R8toA8R8G8B8(const void* sP, s32 sN, void* dP);
	static void convert_B8G8R8A8toA8R8G8B8(const void* sP, s32 sN, void* dP);
};