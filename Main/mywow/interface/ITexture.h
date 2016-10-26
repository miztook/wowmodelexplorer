#pragma once

#include "core.h"
#include "IResourceCache.h"

enum E_TEXTURE_TYPE
{
	ETT_IMAGE = 0,
	ETT_RENDERTARGET,
	ETT_DEPTHSTENCIL,

	ETT_COUNT
};

class ITexture : public IReferenceCounted<ITexture>
{
protected:
	virtual void onRemove() 
	{
		releaseVideoTexture();
	}
	virtual ~ITexture() { }

public:
	ITexture() : TextureSize(0,0), ColorFormat(ECF_UNKNOWN), Type(ETT_IMAGE), 
		SampleCount(1), HasMipMaps(false), VideoBuilt(false), NumMipmaps(1) {}
	
public:
	const dimension2du& getSize() const { return TextureSize; }
	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }
	bool hasMipMaps() const { return HasMipMaps; }
	u32 getNumMipmaps() const { return NumMipmaps; }
	u8 getSampleCount() const { return SampleCount; }
	E_TEXTURE_TYPE getType() const { return (E_TEXTURE_TYPE)Type; }

	virtual bool isValid() const = 0;

	//video memory
	virtual bool createVideoTexture() = 0;
	virtual void releaseVideoTexture() = 0;

protected:
	dimension2du	TextureSize;
	ECOLOR_FORMAT	ColorFormat;
	u32		NumMipmaps;
	u8	Type;
	u8	SampleCount;
	bool	HasMipMaps;	
	bool VideoBuilt;
};