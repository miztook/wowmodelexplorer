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

#ifdef _MSC_VER
#	pragma pack (pop)
#else
#	pragma pack ()
#endif

enum KTX_MetadataKey
{
	KTX_METADATA_ALPHAINFO = 0,
};