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
	bool convertBlpToPVR(IBLPImage* blp, const c8* outputpath);
	const c8* getLastErrorString() const { return strError.c_str(); }

private:
	bool processPvrTexture(pvrtexture::CPVRTexture* pTexture, IBLPImage* blpImage, const c8* outputpath, bool forceUnCompress);

	c8* getPvrPixelFormatString(pvrtexture::PixelType type);

private:
	PVRCompressionQuality Quality;
	string1024	strError;
};