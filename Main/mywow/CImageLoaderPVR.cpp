#include "stdafx.h"
#include "CImageLoaderPVR.h"
#include "mywow.h"
#include "pvrlib.h"
#include "CImage.h"
#include "CPVRImage.h"

IImage* CImageLoaderPVR::loadAsImage( IMemFile* file )
{
	u8* buffer = (u8*)file->getBuffer();

	u32 width = 0;
	u32 height = 0;
	u32 offset = 0;

	ECOLOR_FORMAT format = ECF_UNKNOWN;

	CPVRImage::PVRTextureHeaderV3* headerV3 = reinterpret_cast<CPVRImage::PVRTextureHeaderV3*>(buffer);

	if (headerV3->u32Version == FOURCC('P','V','R', 3))		//version 3
	{	
		offset = sizeof(CPVRImage::PVRTextureHeaderV3);

		width = headerV3->u32Width;
		height = headerV3->u32Height;

		static const u64 PVRTEX_PFHIGHMASK=0xffffffff00000000ull;

		u32 formatFlags = (u32)((headerV3->u64PixelFormat & PVRTEX_PFHIGHMASK) >> 32);
		if (formatFlags == 0)
		{
			switch (headerV3->u64PixelFormat)
			{
			case CPVRImage::ePVRTPF_PVRTCI_2bpp_RGB:
				format = ECF_PVRTC1_RGB_2BPP;
				break;
			case CPVRImage::ePVRTPF_PVRTCI_2bpp_RGBA:
				format = ECF_PVRTC1_RGBA_2BPP;
				break;
			case CPVRImage::ePVRTPF_PVRTCI_4bpp_RGB:
				format = ECF_PVRTC1_RGB_4BPP;
				break;
			case CPVRImage::ePVRTPF_PVRTCI_4bpp_RGBA:
				format = ECF_PVRTC1_RGBA_4BPP;
				break;
			default:
				ASSERT(false);
				break;
			}
		}
		else		//uncompressed
		{
			CPVRImage::EPVRTVariableType channelType = (CPVRImage::EPVRTVariableType)headerV3->u32ChannelType;

			switch(channelType)
			{
			case CPVRImage::ePVRTVarTypeUnsignedByteNorm:
				{
					switch(headerV3->u64PixelFormat)
					{
					case PVR_GEN_PIXEL_ID4('r','g','b','a',8,8,8,8):
						format = ECF_A8R8G8B8;
						break;
					case PVR_GEN_PIXEL_ID4('r','g','b',0,8,8,8,0):
						format = ECF_R8G8B8;
						break;
					}
				}
				break;
			default:
				{
					ASSERT(false);			//not supported format
					return NULL_PTR;
				}
			}
		}

		if (headerV3->u32MetaDataSize)
		{
			offset += headerV3->u32MetaDataSize;
		}
	}
	else		//old format
	{
		CPVRImage::PVR_Texture_Header*	headerV2 = reinterpret_cast<CPVRImage::PVR_Texture_Header*>(buffer);

		if (headerV2->dwPVR  != FOURCC('P', 'V', 'R', '!'))
		{
			ASSERT(false);
			return NULL_PTR;
		}

		offset = headerV2->dwHeaderSize;
		width = headerV2->dwWidth;
		height = headerV2->dwHeight;

		const u32 formatFlags = headerV2->dwpfFlags & PVR_TEXTURE_FLAG_TYPE_MASK;
		switch (formatFlags)
		{
		case CPVRImage::OGL_PVRTC2:
			{
				format = headerV2->dwAlphaBitMask ? ECF_PVRTC1_RGBA_2BPP : ECF_PVRTC1_RGB_2BPP;
			}
			break;
		case CPVRImage::OGL_PVRTC4:
			{
				format = headerV2->dwAlphaBitMask ? ECF_PVRTC1_RGBA_4BPP : ECF_PVRTC1_RGB_4BPP;
			}
			break;
		case CPVRImage::OGL_RGBA_8888:
			{
				format = ECF_A8R8G8B8;
			}
			break;
		case CPVRImage::OGL_RGB_888:
			{
				format = ECF_R8G8B8;
			}
			break;
		default:
			{
				ASSERT(false);			//not supported format
				return NULL_PTR;
			}
		}
	}

	u8* src = buffer + offset;

	dimension2du dim(width, height);
	u32* decompressed = new u32[width * height];

	switch(format)
	{
	case ECF_PVRTC1_RGB_2BPP:
		PVRDecompressRGB_2bpp(src, width, height, (u8*)decompressed);
		break;
	case ECF_PVRTC1_RGBA_2BPP:
		PVRDecompressRGBA_2bpp(src, width, height, (u8*)decompressed);
		break;
	case ECF_PVRTC1_RGB_4BPP:
		PVRDecompressRGB_4bpp(src, width, height, (u8*)decompressed);
		break;
	case ECF_PVRTC1_RGBA_4BPP:
		PVRDecompressRGBA_4bpp(src, width, height, (u8*)decompressed);
		break;
	case ECF_A8R8G8B8:
		{
			for (u32 i=0; i<width*height; ++i)
			{
				decompressed[i*4] = src[i*4];
				decompressed[i*4+1] = src[i*4+3];
				decompressed[i*4+2] = src[i*4+2];
				decompressed[i*4+3] = src[i*4+1];
			}
		}
		break;
	default:
		delete[] decompressed;
		ASSERT(false);
		return NULL_PTR;
	}

	CImage* image = new CImage(ECF_A8R8G8B8, dim, decompressed, true);
	return image;
}