#pragma once

#include "core.h"
#include "baseDefine.h"

class IBLPImage;

namespace pvrtexture
{
	class CPVRTexture;
	union PixelType;
}

class CBlpToKTX
{
public:
	explicit CBlpToKTX(PVRCompressionQuality quality);

public:
	bool convertBlpToKTX(IBLPImage* blp, const c8* outputpath);
	const c8* getLastErrorString() const { return strError.c_str(); }

private:
	bool processPvrTexture(pvrtexture::CPVRTexture* pTexture, IBLPImage* blpImage, const c8* outputpath);

	bool addAlphaMetadata(const c8* filename);

private:
	PVRCompressionQuality Quality;
	string1024	strError;
};