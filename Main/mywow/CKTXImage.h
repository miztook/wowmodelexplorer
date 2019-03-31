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
	virtual bool fromImageData(const uint8_t* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap);
	virtual const void* getMipmapData(uint32_t level) const;  
	virtual bool copyMipmapData(uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height);

private:
	uint8_t*			FileData;
	uint32_t			MipmapOffset[16];

public:

#	pragma pack (1)

	struct KTX_Header
	{
		uint8_t identifier[12];
		uint32_t endianness;
		uint32_t glType;
		uint32_t glTypeSize;
		uint32_t glFormat;
		uint32_t glInternalFormat;
		uint32_t glBaseInternalFormat;
		uint32_t pixelWidth;
		uint32_t pixelHeight;
		uint32_t pixelDepth;
		uint32_t numberOfArrayElements;
		uint32_t numberOfFaces;
		uint32_t numberOfMipmapLevels;
		uint32_t bytesOfKeyValueData;
	};

	struct KTX_Metadata
	{
		uint32_t byteSize;
		uint32_t key;
		uint32_t data;
	};

#	pragma pack ()

	enum KTX_MetadataKey
	{
		KTX_METADATA_ALPHAINFO = 0,
	};
};