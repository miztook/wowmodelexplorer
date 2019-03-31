#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "IVideoResource.h"

enum E_TEXTURE_TYPE
{
	ETT_IMAGE = 0,
	ETT_RENDERTARGET,
	ETT_DEPTHSTENCIL,

	ETT_COUNT
};

class ITexture : public IReferenceCounted<ITexture>, public IVideoResource
{
protected:
	virtual ~ITexture() { }

public:
	explicit ITexture(bool mipmap) : HasMipMaps(mipmap), TextureSize(0, 0), Type(ETT_IMAGE), VideoBuilt(false)
	{
		ColorFormat = ECF_UNKNOWN;
		SampleCount = 1;
		NumMipmaps = 1;
	}
public:
	virtual bool isValid() const = 0;

public:
	const dimension2du& getSize() const { return TextureSize; }
	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }
	bool hasMipMaps() const { return HasMipMaps; }
	uint32_t getNumMipmaps() const { return NumMipmaps; }
	uint8_t getSampleCount() const { return SampleCount; }
	E_TEXTURE_TYPE getType() const { return (E_TEXTURE_TYPE)Type; }

protected:
	dimension2du	TextureSize;
	ECOLOR_FORMAT	ColorFormat;
	uint32_t		NumMipmaps;
	uint8_t	Type;
	uint8_t	SampleCount;
	const bool	HasMipMaps;	
	bool	VideoBuilt;
};