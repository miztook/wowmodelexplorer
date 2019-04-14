#pragma once

#include "core.h"
#include "baseDefine.h"

class IBLPImage;

namespace pvrtexture
{
	class CPVRTexture;
	union PixelType;
}

class CBlpToPVR
{
public:
	explicit CBlpToPVR(PVRCompressionQuality quality);

public:
	bool convertBlpToPVR(IBLPImage* blp, const char* outputpath);
	const char* getLastErrorString() const { return strError.c_str(); }

private:
	bool processPvrTexture(pvrtexture::CPVRTexture* pTexture, IBLPImage* blpImage, const char* outputpath, bool forceUnCompress);

	char* getPvrPixelFormatString(pvrtexture::PixelType type);

private:
	PVRCompressionQuality Quality;
	string1024	strError;
};