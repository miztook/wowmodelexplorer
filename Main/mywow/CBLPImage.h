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
	virtual bool loadFile(IMemFile* file);	
	virtual bool fromImageData(const uint8_t* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap);
	virtual const void* getMipmapData(uint32_t level) const;  
	virtual bool copyMipmapData(uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height);

private:
	void copy16BitMipMap(const uint8_t* src, uint8_t* tgt,
		uint32_t width, uint32_t height,  uint32_t pitchsrc, uint32_t pitchtgt) const;

private:
	uint8_t*			FileData;

public:

#	pragma pack (1)

	//------------------------------------------------------------------------------
	struct SBLPHeader { 				    
		/*0x00*/    uint32_t      magic;           	     // always 'BLP2'
		/*0x04*/    uint32_t  _version;            	     // Version, always 1
		/*0x08*/    uint8_t   _compress;           	     // Compression: 1 for uncompressed, 2 for DXTC
		/*0x09*/    uint8_t   _alphaDepth;         	     // Alpha channel bit depth: 0, 1 or 8
		/*0x0A*/    uint8_t   _alphaCompress;      	     // Something about alpha? (compressed, alpha:0 or 1): 0, (compressed, alpha:8): 1, uncompressed: 2,4 or 8 (mostly 8)
		/*0x0B*/    uint8_t   _mipmap;             	     // when 0 there is only 1 mipmaplevel. compressed: 0, 1 or 2, uncompressed: 1 or 2 (mostly 1)
		/*0x0C*/    uint32_t  _xres;	           	     // X resolution (power of 2)
		/*0x10*/    uint32_t  _yres;	         	     // Y resolution (power of 2)
		/*0x14*/    uint32_t  _mipmapOfs[16];                // offsets for every mipmap level (or 0 when there is no more mipmap level)
		/*0x54*/    uint32_t  _mipmapSize[16];   	     // sizes for every mipmap level (or 0 when there is no more mipmap level) 
	};

#	pragma pack ()
};
