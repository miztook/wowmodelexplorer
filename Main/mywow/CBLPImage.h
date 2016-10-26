#pragma once

#include "core.h"
#include "IBLPImage.h"

class CBLPImage : public IBLPImage
{
private:
	DISALLOW_COPY_AND_ASSIGN(CBLPImage);

public:
	CBLPImage();
	~CBLPImage();

public:
	virtual bool loadFile(IMemFile* file, bool abgr);	
	virtual bool fromImageData(const u8* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap);
	virtual const void* getMipmapData(u32 level) const;  
	virtual bool copyMipmapData(u32 level, void* dest, u32 pitch, u32 width, u32 height);

private:
	void copy16BitMipMap(const u8* src, u8* tgt,
		u32 width, u32 height,  u32 pitchsrc, u32 pitchtgt) const;

private:
	u8*			FileData;

public:

#	pragma pack (1)

	//------------------------------------------------------------------------------
	struct SBLPHeader { 				    
		/*0x00*/    u32      magic;           	     // always 'BLP2'
		/*0x04*/    u32  _version;            	     // Version, always 1
		/*0x08*/    u8   _compress;           	     // Compression: 1 for uncompressed, 2 for DXTC
		/*0x09*/    u8   _alphaDepth;         	     // Alpha channel bit depth: 0, 1 or 8
		/*0x0A*/    u8   _alphaCompress;      	     // Something about alpha? (compressed, alpha:0 or 1): 0, (compressed, alpha:8): 1, uncompressed: 2,4 or 8 (mostly 8)
		/*0x0B*/    u8   _mipmap;             	     // when 0 there is only 1 mipmaplevel. compressed: 0, 1 or 2, uncompressed: 1 or 2 (mostly 1)
		/*0x0C*/    u32  _xres;	           	     // X resolution (power of 2)
		/*0x10*/    u32  _yres;	         	     // Y resolution (power of 2)
		/*0x14*/    u32  _mipmapOfs[16];                // offsets for every mipmap level (or 0 when there is no more mipmap level)
		/*0x54*/    u32  _mipmapSize[16];   	     // sizes for every mipmap level (or 0 when there is no more mipmap level) 
	};

#	pragma pack ()
};
