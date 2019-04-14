#pragma once

#include "base.h"

enum PVRCompressionQuality
{
	PVRCQ_Low		= 0,
	PVRCQ_Normal	= 1,
	PVRCQ_High		= 2,
	PVRCQ_Best		= 3,
};

#ifdef _MSC_VER
#	pragma pack (push, 1)
#else
#	pragma pack (1)
#endif

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

#ifdef _MSC_VER
#	pragma pack (pop)
#else
#	pragma pack ()
#endif

enum KTX_MetadataKey
{
	KTX_METADATA_ALPHAINFO = 0,
};