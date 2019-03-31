#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "SColor.h"

class IMemFile;
class IImage;

//可能的格式 DXT1, DXT3, DXT5, A8R8G8B8

class IBLPImage : public IReferenceCounted<IBLPImage>
{
protected:
	virtual ~IBLPImage() {}

public:
	IBLPImage()
	{
		Format = ECF_UNKNOWN;
		NumMipMaps = 1;
		AlphaDepth = 0;
		memset(MipmapDataSize, 0, sizeof(MipmapDataSize));
		memset(MipmapPitch, 0, sizeof(MipmapPitch));
		IsABGR = false;
	}

public:
	virtual bool loadFile(IMemFile* file, bool abgr) = 0;
	virtual bool fromImageData(const uint8_t* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap) = 0;
	virtual const void* getMipmapData(uint32_t level) const = 0;  
	virtual bool copyMipmapData(uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height) = 0;

	ECOLOR_FORMAT getColorFormat() const { return Format; }
	const dimension2du& getDimension() const { return Size; }
	uint32_t getNumMipLevels() const { return NumMipMaps; }
	uint32_t getMipmapDataSize(uint32_t level) const { return MipmapDataSize[level]; }
	uint32_t getMipmapPitch(uint32_t level) const { return MipmapPitch[level]; }
	uint32_t getAlphaDepth() const { return AlphaDepth; }
	bool isABGR() const { return IsABGR; }

protected:
	dimension2du Size;
	uint32_t			NumMipMaps;	
	uint32_t			MipmapDataSize[16];
	uint32_t			MipmapPitch[16];	
	uint32_t			AlphaDepth;
	ECOLOR_FORMAT	Format;
	bool			IsABGR;
};
