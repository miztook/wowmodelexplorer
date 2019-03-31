#include "stdafx.h"
#include "CImageLoaderKTX.h"
#include "mywow.h"
#include "CImage.h"
#include "CKTXImage.h"

IImage* CImageLoaderKTX::loadAsImage( IMemFile* file )
{
	uint8_t* buffer = (uint8_t*)file->getBuffer();

	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t offset = 0;

	ECOLOR_FORMAT format = ECF_UNKNOWN;

	CKTXImage::KTX_Header header;
	Q_memcpy(&header, sizeof(CKTXImage::KTX_Header), buffer, sizeof(CKTXImage::KTX_Header));
	offset = sizeof(CKTXImage::KTX_Header);

	const uint8_t KTX_IDENTIFIER[12] = {0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

	if (memcmp(header.identifier, KTX_IDENTIFIER, 12) != 0)
	{
		ASSERT(false);
		return nullptr;
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
			return nullptr;
		}
	}

	uint8_t* src = buffer + offset + sizeof(uint32_t);

	dimension2du dim(width, height);
	uint32_t* decompressed = new uint32_t[width * height];

	switch(format)
	{
	case ECF_A8R8G8B8:
		{
			for (uint32_t i=0; i<width*height; ++i)
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
		return nullptr;
	}

	CImage* image = new CImage(ECF_A8R8G8B8, dim, decompressed, true);
	return image;
}
