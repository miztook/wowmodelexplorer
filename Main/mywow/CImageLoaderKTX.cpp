#include "stdafx.h"
#include "CImageLoaderKTX.h"
#include "mywow.h"
#include "CImage.h"
#include "CKTXImage.h"

IImage* CImageLoaderKTX::loadAsImage( IMemFile* file )
{
	u8* buffer = (u8*)file->getBuffer();

	u32 width = 0;
	u32 height = 0;
	u32 offset = 0;

	ECOLOR_FORMAT format = ECF_UNKNOWN;

	CKTXImage::KTX_Header header;
	Q_memcpy(&header, sizeof(CKTXImage::KTX_Header), buffer, sizeof(CKTXImage::KTX_Header));
	offset = sizeof(CKTXImage::KTX_Header);

	const u8 KTX_IDENTIFIER[12] = {0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

	if (memcmp(header.identifier, KTX_IDENTIFIER, 12) != 0)
	{
		ASSERT(false);
		return NULL_PTR;
	}

	width = header.pixelWidth;
	height = header.pixelHeight;

	switch(header.glInternalFormat)
	{
	case GL_RGBA8:
		{
			format = ECF_A8R8G8B8;
			if (header.bytesOfKeyValueData > 0)
			{
				offset += header.bytesOfKeyValueData;
			}
		}
		break;
	default:
		{
			ASSERT(false);			//not supported format
			return NULL_PTR;
		}
	}

	u8* src = buffer + offset + sizeof(u32);

	dimension2du dim(width, height);
	u32* decompressed = new u32[width * height];

	switch(format)
	{
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
