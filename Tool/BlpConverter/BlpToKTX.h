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
	bool convertBlpToKTX(IBLPImage* blp, const char* outputpath);
	const char* getLastErrorString() const { return strError.c_str(); }

private:
	bool processPvrTexture(pvrtexture::CPVRTexture* pTexture, IBLPImage* blpImage, const char* outputpath);

	bool addAlphaMetadata(const char* filename);

private:
	PVRCompressionQuality Quality;
	string1024	strError;
};