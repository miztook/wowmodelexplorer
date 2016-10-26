#include "stdafx.h"
#include "TGAImageWriter.h"
#include "core.h"
#include <cstdio>

bool TGAWriteFile( const char* pszFilePath, unsigned int nWidth, unsigned int nHeight, TGAFormat eFormat, const void* pPixelData )
{
	TgaHeader header;

	if (nWidth == 0 || nHeight == 0 || nWidth > 0xFFFF || nHeight > 0xFFFF)
	{
		_ASSERT(false);
		return false;
	}

	header.IdLength = 0;			// No special ID for image.
	header.CmapType = 0;			// No pallet map.
	header.ImageType = TGA_BGR;		// Uncompressed true color bitmap.

	header.CmapIndex = 0;
	header.CmapLength = 0;
	header.CmapEntrySize = 0;

	// Image origin and size.
	header.X_Origin = 0;
	header.Y_Origin = 0;

	header.ImageWidth = nWidth;
	header.ImageHeight = nHeight;

	// Pixel depth.
	unsigned int nPixelBytes = 0;
	switch (eFormat)
	{
	case TGA_FORMAT_BGR:
		{
			nPixelBytes = 3;
			break;
		}
	case TGA_FORMAT_BGRA:
		{
			nPixelBytes = 4;
			break;
		}
	default:
		{
			return false;
		}
	}

	header.PixelDepth = nPixelBytes * 8;

	// Bits 4 & 5 of the Image Descriptor byte control the X & Y ordering of the pixels. First line is at top.
	// Bits 0 - 3 for number of alpha bits = 8 bits.
	header.ImagDesc = 8 | 32;

	FILE* ImageFile = NULL;
	fopen_s(&ImageFile, pszFilePath,"wb");
	if (!ImageFile)
	{
		return false;
	}

	int headerSize = sizeof(header);
	if(fwrite(&header,1,sizeof(header),ImageFile) != sizeof(header))
	{
		return false;
	}

	const unsigned int nPixelDataLength = nPixelBytes * nWidth * nHeight;
	if(fwrite(pPixelData,1,nPixelDataLength,ImageFile) != nPixelDataLength)
	{
		return false;
	}

	TgaFooter footer;
	footer.ExtensionOffset = 0;
	footer.DeveloperAreaOffset = 0;
	strcpy_s(footer.Signature, "TRUEVISION-XFILE.");

	if(fwrite(&footer,1,sizeof(footer),ImageFile) != sizeof(footer))
	{
		return false;
	}

	fclose(ImageFile);

	return true;
}
