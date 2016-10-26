#pragma once

enum TGAFormat
{
	TGA_FORMAT_UNKNOWN = 0,

	TGA_FORMAT_BGR,
	TGA_FORMAT_BGRA,
};

#pragma pack(1)

struct TgaHeader
{
	unsigned char   	IdLength;            	// Image ID Field Length
	unsigned char   	CmapType;            	// Color Map Type
	unsigned char   	ImageType;           	// Image Type

	unsigned short		CmapIndex;           	// First Entry Index
	unsigned short		CmapLength;          	// Color Map Length
	unsigned char		CmapEntrySize;       	// Color Map Entry Size

	unsigned short		X_Origin;            	// X-origin of Image
	unsigned short		Y_Origin;            	// Y-origin of Image
	unsigned short		ImageWidth;          	// Image Width
	unsigned short		ImageHeight;         	// Image Height
	unsigned char		PixelDepth;          	// Pixel Depth
	unsigned char		ImagDesc;            	// Image Descriptor
};

struct TgaFooter
{
	unsigned int		ExtensionOffset;		// Offset of extension section
	unsigned int		DeveloperAreaOffset;	// Offset of developer area section
	char				Signature[18];			// "TRUEVISION-XFILE."
};

#pragma pack()

static const unsigned char	TGA_Map			= 1;
static const unsigned char	TGA_BGR			= 2;
static const unsigned char	TGA_Mono		= 3;
static const unsigned char	TGA_RLEMap		= 9;
static const unsigned char	TGA_RLERGB		= 10;
static const unsigned char	TGA_RLEMono		= 11;

bool TGAWriteFile(const char* pszFilePath, unsigned int nWidth, unsigned int nHeight, TGAFormat eFormat, const void* pPixelData);