#include "stdafx.h"
#include "SColor.h"
#include "pvrlib.h"

/* dependencies */
#ifdef MW_COMPILE_WITH_GLES2

// #include "PVRTextureHeader.h"
// #include "PVRTexture.h"
// #include "PVRTextureUtilities.h"
// #include "PVRTDecompress.h"

//using namespace pvrtexture;

#endif


int PVRDecompressRGB_2bpp( unsigned char *src, int width, int height, unsigned char *dest )
{
// #ifdef MW_COMPILE_WITH_GLES2
// 	PVRTDecompressPVRTC(src, 1, width, height, dest);
// 	return 0;
// #else
	ASSERT(false);
	return 0;
//#endif
}

int PVRDecompressRGBA_2bpp( unsigned char *src, int width, int height, unsigned char *dest )
{
// #ifdef MW_COMPILE_WITH_GLES2
// 	PVRTDecompressPVRTC(src, 1, width, height, dest);
// 	return 0;
// #else
	ASSERT(false);
	return 0;
//#endif
}

int PVRDecompressRGB_4bpp( unsigned char *src, int width, int height, unsigned char *dest )
{
// #ifdef MW_COMPILE_WITH_GLES2
// 
// 	PVRTDecompressPVRTC(src, 0, width, height, dest);
// 	return 0;
// #else
	ASSERT(false);
	return 0;
//#endif
}

int PVRDecompressRGBA_4bpp( unsigned char *src, int width, int height, unsigned char *dest )
{
// #ifdef MW_COMPILE_WITH_GLES2
// 
// 	PVRTDecompressPVRTC(src, 0, width, height, dest);
// 	return 0;
// #else
	ASSERT(false);
	return 0;
//#endif
}

int PVRCompressA8R8G8B8ToRGB4bpp( unsigned char* src, int width, int height, unsigned char* dest )
{
// #ifdef MW_COMPILE_WITH_GLES2
// 
// 	CPVRTextureHeader PVRHeader(PixelType('r','g','b','a',8,8,8,8).PixelTypeID, height, width, 1, 1, 1, 1);
// 
// 	CPVRTexture* pTexture = new CPVRTexture(PVRHeader, src);
// 
// 	if(!Transcode(*pTexture, PixelType(ePVRTPF_PVRTCI_4bpp_RGB), ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB, ePVRTCFastest))
// 	{
// 		delete pTexture;
// 		ASSERT(false);
// 		return -1;
// 	}
// 
// 	u32 pitch, dstSize;
// 	getImagePitchAndBytes(ECF_PVRTC1_RGB_4BPP, width, height, pitch, dstSize);
// 	if (pTexture->getDataSize() < dstSize)
// 	{
// 		delete pTexture;
// 		ASSERT(false);
// 		return -1;
// 	}
// 
// 	memcpy_s(dest, dstSize, pTexture->getDataPtr(), dstSize);
// 
// 	delete pTexture;
// 	return dstSize;
// #else
	ASSERT(false);
	return 0;
//#endif
}
