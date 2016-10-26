#pragma once

#include "core.h"
#include "IKTXImage.h"

#ifndef GL_ETC1_RGB8_OES
#define GL_ETC1_RGB8_OES 0x8D64
#endif

#ifndef GL_RGB8
#define GL_RGB8	0x8051
#endif

#ifndef GL_RGBA8
#define GL_RGBA8	0x8058
#endif

/* GL_AMD_compressed_ATC_texture */
#ifndef GL_AMD_compressed_ATC_texture
#define GL_ATC_RGB_AMD                                          0x8C92
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD                          0x8C93
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD                      0x87EE
#endif

class CKTXImage : public IKTXImage
{
private:
	DISALLOW_COPY_AND_ASSIGN(CKTXImage);

public:
	CKTXImage();
	~CKTXImage();

public:
	virtual bool loadFile(IMemFile* file);
	virtual bool fromImageData(const u8* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap);
	virtual const void* getMipmapData(u32 level) const;  
	virtual bool copyMipmapData(u32 level, void* dest, u32 pitch, u32 width, u32 height);

private:
	u8*			FileData;
	u32			MipmapOffset[16];

public:

#	pragma pack (1)

	struct KTX_Header
	{
		u8 identifier[12];
		u32 endianness;
		u32 glType;
		u32 glTypeSize;
		u32 glFormat;
		u32 glInternalFormat;
		u32 glBaseInternalFormat;
		u32 pixelWidth;
		u32 pixelHeight;
		u32 pixelDepth;
		u32 numberOfArrayElements;
		u32 numberOfFaces;
		u32 numberOfMipmapLevels;
		u32 bytesOfKeyValueData;
	};

	struct KTX_Metadata
	{
		u32 byteSize;
		u32 key;
		u32 data;
	};

#	pragma pack ()

	enum KTX_MetadataKey
	{
		KTX_METADATA_ALPHAINFO = 0,
	};
};