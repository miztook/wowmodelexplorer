#include "stdafx.h"
#include "CPVRImage.h"
#include "mywow.h"
#include "pvrlib.h"
#include "CBlit.h"

// V3 Header Identifiers.
const uint32_t PVRTEX3_IDENT			= 0x03525650;	// 'P''V''R'3

// If endianness is backwards then PVR3 will read as 3RVP, hence why it is written as an int.

const uint32_t ALPHAINFO_ID  = 0x48504C41;     // 'A''L''P''H'
const uint32_t ALPHAINFO_KEY = 0x544E4955;     // 'U''I''N''T'

//A 64 bit pixel format ID & this will give you the high bits of a pixel format to check for a compressed format.
static const uint64_t PVRTEX_PFHIGHMASK=0xffffffff00000000ull;

//unused
/*
const uint32_t PVRTEX3_IDENT_REV		= 0x50565203;
 
//Current version texture identifiers
const uint32_t PVRTEX_CURR_IDENT		= PVRTEX3_IDENT;
const uint32_t PVRTEX_CURR_IDENT_REV	= PVRTEX3_IDENT_REV;

// PVR Header file flags.										Condition if true. If false, opposite is true unless specified.
const uint32_t PVRTEX3_FILE_COMPRESSED	= (1<<0);		//	Texture has been file compressed using PVRTexLib (currently unused)
const uint32_t PVRTEX3_PREMULTIPLIED		= (1<<1);		//	Texture has been premultiplied by alpha value.	

// Mip Map level specifier constants. Other levels are specified by 1,2...n
const int32_t PVRTEX_TOPMIPLEVEL			= 0;
const int32_t PVRTEX_ALLMIPLEVELS		= -1; //This is a special number used simply to return a total of all MIP levels when dealing with data sizes.


//Legacy constants (V1/V2)

const uint32_t PVRTEX_MIPMAP				= (1<<8);		// has mip map levels
const uint32_t PVRTEX_TWIDDLE			= (1<<9);		// is twiddled
const uint32_t PVRTEX_BUMPMAP			= (1<<10);		// has normals encoded for a bump map
const uint32_t PVRTEX_TILING				= (1<<11);		// is bordered for tiled pvr
const uint32_t PVRTEX_CUBEMAP			= (1<<12);		// is a cubemap/skybox
const uint32_t PVRTEX_FALSEMIPCOL		= (1<<13);		// are there false coloured MIP levels
const uint32_t PVRTEX_VOLUME				= (1<<14);		// is this a volume texture
const uint32_t PVRTEX_ALPHA				= (1<<15);		// v2.1 is there transparency info in the texture
const uint32_t PVRTEX_VERTICAL_FLIP		= (1<<16);		// v2.1 is the texture vertically flipped

const uint32_t PVRTEX_PIXELTYPE			= 0xff;			// pixel type is always in the last 16bits of the flags
const uint32_t PVRTEX_IDENTIFIER			= 0x21525650;	// the pvr identifier is the characters 'P','V','R'

const uint32_t PVRTEX_V1_HEADER_SIZE		= 44;			// old header size was 44 for identification purposes
 */

CPVRImage::CPVRImage()
{
	FileData = nullptr;
	memset(MipmapOffset, 0, sizeof(MipmapOffset));
}

CPVRImage::~CPVRImage()
{
	delete[] FileData;
}

bool CPVRImage::loadFile( IMemFile* file )
{
	uint32_t filesize = file->getSize();
	FileData = new uint8_t[filesize];
	Q_memcpy(FileData, filesize, file->getBuffer(), filesize);

	uint32_t offset = 0;
	PVRTextureHeaderV3* headerV3 = reinterpret_cast<PVRTextureHeaderV3*>(FileData);

	if (headerV3->u32Version == PVRTEX3_IDENT)		//version 3
	{	
		offset = sizeof(PVRTextureHeaderV3);
		
		uint32_t formatFlags = (uint32_t)((headerV3->u64PixelFormat & PVRTEX_PFHIGHMASK) >> 32);
		if (formatFlags == 0)
		{
			switch (headerV3->u64PixelFormat)
			{
			case ePVRTPF_PVRTCI_2bpp_RGB:
				Format = ECF_PVRTC1_RGB_2BPP;
				break;
			case ePVRTPF_PVRTCI_2bpp_RGBA:
				Format = ECF_PVRTC1_RGBA_2BPP;
				break;
			case ePVRTPF_PVRTCI_4bpp_RGB:
				Format = ECF_PVRTC1_RGB_4BPP;
				break;
			case ePVRTPF_PVRTCI_4bpp_RGBA:
				Format = ECF_PVRTC1_RGBA_4BPP;
				break;
			case ePVRTPF_ETC1:
				{
					Format = ECF_ETC1_RGB;
					if (headerV3->u32MetaDataSize)
					{
						uint32_t alphaInfo[4];
						Q_memcpy(alphaInfo, sizeof(alphaInfo), FileData+offset, sizeof(alphaInfo));
						if (alphaInfo[0] == ALPHAINFO_ID &&
							alphaInfo[1] == ALPHAINFO_KEY &&
							alphaInfo[3] == 1)
						{
							Format = ECF_ETC1_RGBA;
						}
					}
				}
				break;
			default:
				ASSERT(false);			//not supported format
				return false;
			}
		}
		else		//uncompressed
		{
			EPVRTVariableType channelType = (EPVRTVariableType)headerV3->u32ChannelType;
			switch(channelType)
			{
			case ePVRTVarTypeUnsignedByteNorm:
				{
					switch(headerV3->u64PixelFormat)
					{
					case PVR_GEN_PIXEL_ID4('r','g','b','a',8,8,8,8):
						Format = ECF_A8R8G8B8;
						break;
					case PVR_GEN_PIXEL_ID4('r','g','b',0,8,8,8,0):
						Format = ECF_R8G8B8;
						break;
					}
				}
				break;
			default:
				{
					ASSERT(false);			//not supported format
					return false;
				}
			}
		}

		Size.Width = headerV3->u32Width;
		Size.Height = headerV3->u32Height;
		NumMipMaps = headerV3->u32MIPMapCount;

		if (headerV3->u32MetaDataSize)
		{
			offset += headerV3->u32MetaDataSize;
		}
	}
	else			//legacy version 2
	{
		PVR_Texture_Header*	headerV2 = reinterpret_cast<PVR_Texture_Header*>(FileData);

		if (headerV2->dwPVR  != FOURCC('P', 'V', 'R', '!'))
		{
			ASSERT(false);
			return false;
		}
		offset += headerV2->dwHeaderSize;

		const uint32_t formatFlags = headerV2->dwpfFlags & PVR_TEXTURE_FLAG_TYPE_MASK;
		switch (formatFlags)
		{
		case OGL_PVRTC2:
			{
				Format = headerV2->dwAlphaBitMask ? ECF_PVRTC1_RGBA_2BPP : ECF_PVRTC1_RGB_2BPP;
			}
			break;
		case OGL_PVRTC4:
			{
				Format = headerV2->dwAlphaBitMask ? ECF_PVRTC1_RGBA_4BPP : ECF_PVRTC1_RGB_4BPP;
			}
			break;
		case OGL_RGBA_8888:
			{
				Format = ECF_A8R8G8B8;
			}
			break;
		case OGL_RGB_888:
			{
				Format = ECF_R8G8B8;
			}
			break;
		default:
			{
				ASSERT(false);			//not supported format
				return false;
			}
		}

		Size.Width = headerV2->dwWidth;
		Size.Height = headerV2->dwHeight;
		NumMipMaps = headerV2->dwMipMapCount + 1;
	}

	if (isCompressedFormat(Format) && 
		(Size.Width != Size.Height || !isPOT(Size.Width)))
	{
		ASSERT(false);
		return false;
	}

	uint32_t mipoffset = offset;
	for (uint32_t i=0; i<NumMipMaps; ++i)
	{
		MipmapOffset[i] = mipoffset;

		dimension2du mipsize = Size.getMipLevelSize(i);
		getImagePitchAndBytes(Format, mipsize.Width, mipsize.Height, MipmapPitch[i], MipmapDataSize[i]);
		
		mipoffset += MipmapDataSize[i];
	}

	ASSERT(mipoffset == filesize);

	return true;
}

bool CPVRImage::fromImageData( const uint8_t* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap )
{
	ASSERT(false);
	return false;

	/*
	if (size.Width % 4 != 0 || size.Height % 4 != 0 || format != ECF_A8R8G8B8)
	{
		ASSERT(false);
		return false;
	}

	Format = ECF_PVRTC1_RGB_4BPP;
	Size = size;

	PVRTextureHeaderV3 header = {0};
	header.u32Version = PVRTEX3_IDENT;
	header.u64PixelFormat = ePVRTPF_PVRTCI_4bpp_RGB;
	header.u32ColourSpace = ePVRTCSpacelRGB;
	header.u32ChannelType = ePVRTVarTypeUnsignedByteNorm;
	header.u32Width = Size.Width;
	header.u32Height = Size.Height;
	header.u32Depth = 1;
	header.u32NumSurfaces = 1;
	header.u32NumFaces = 1;
	header.u32MIPMapCount = mipmap ? Size.getNumMipLevels() : 1;

	NumMipMaps = header.u32MIPMapCount;
	if (NumMipMaps > 16)
	{
		ASSERT(false);
		return false;
	}

	uint32_t offset = sizeof(PVRTextureHeaderV3);
	for (uint32_t i=0; i<NumMipMaps; ++i)
	{
		dimension2du mipsize = Size.getMipLevelSize(i);

		MipmapOffset[i] = offset;

		getImagePitchAndBytes(Format, mipsize.Width, mipsize.Height, MipmapPitch[i], MipmapDataSize[i]);

		offset += MipmapDataSize[i];
	}

	FileData = new uint8_t[offset];
	Q_memcpy(FileData, sizeof(PVRTextureHeaderV3), &header, sizeof(PVRTextureHeaderV3));

	//
	uint32_t mipmapOfs[16] = {0};
	offset = 0;
	uint32_t bpp = getBytesPerPixelFromFormat(ECF_A8R8G8B8);
	for (uint32_t i=0; i<NumMipMaps; ++i)
	{
		dimension2du s = Size.getMipLevelSize(i);
		mipmapOfs[i] = offset;
		offset += bpp * s.getArea();
	}

    if (!offset)
    {
		ASSERT(false);
		return false;
	}
    
	uint8_t* tmpdata = (uint8_t*)Z_AllocateTempMemory(offset);

	//mip 0 data
	Q_memcpy(tmpdata, bpp * Size.getArea(), src, bpp * Size.getArea());

	//mip 0 data -> dxt1
	uint8_t* dst0 = FileData + MipmapOffset[0];
	if (MipmapDataSize[0] != PVRCompressA8R8G8B8ToRGB4bpp(tmpdata, Size.Width, Size.Height, dst0))
	{
		ASSERT(false);
		Z_FreeTempMemory(tmpdata);
		return false;
	}

	for (uint32_t i=1; i<NumMipMaps; ++i)
	{
		dimension2du upper = Size.getMipLevelSize(i-1);
		dimension2du lower = Size.getMipLevelSize(i);
		const uint8_t* s = tmpdata + mipmapOfs[i-1];
		uint8_t* tgt = tmpdata + mipmapOfs[i];

		//mip i data
		CBlit::copy32BitMipMap(s, tgt, lower.Width, lower.Height, bpp * upper.Width, bpp * lower.Width, ECF_A8R8G8B8);

		//mip i data -> dxt1
		uint8_t* dst = FileData + MipmapOffset[i];
		if (MipmapDataSize[i] != PVRCompressA8R8G8B8ToRGB4bpp(tgt, lower.Width, lower.Height, dst))
		{
			ASSERT(false);
			Z_FreeTempMemory(tmpdata);
			return false;
		}
	}

	Z_FreeTempMemory(tmpdata);

	return true;
	*/
}

const void* CPVRImage::getMipmapData( uint32_t level ) const
{
	if (level >= 16)
		return nullptr;

	if (MipmapDataSize[level])
		return FileData +MipmapOffset[level];

	return nullptr;
}

bool CPVRImage::copyMipmapData( uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height )
{
	uint32_t limit = getMipmapDataSize(level);
	const uint8_t* src = static_cast<const uint8_t*>(getMipmapData(level));
	if (!src || !limit )
	{
		ASSERT(false);
		return false;
	}

	uint32_t srcPitch, srcDataSize;
	getImagePitchAndBytes(Format, width, height, srcPitch, srcDataSize);
	if (srcPitch != pitch)
	{
		ASSERT(false);
		return false;
	}

	Q_memcpy(dest, srcDataSize, src, srcDataSize);

	return true;
}

void CPVRImage::copy32BitMipMap( const uint8_t* src, uint8_t* tgt, uint32_t width, uint32_t height, uint32_t pitchsrc, uint32_t pitchtgt ) const
{
	for (uint32_t y=0; y<height; ++y)
	{
		for (uint32_t x=0; x<width; ++x)
		{
			uint32_t a=0, r=0, g=0, b=0;
			SColor c;

			for (int32_t dy=0; dy<2; ++dy)
			{
				const int32_t tgy = (y*2)+dy;
				for (int32_t dx=0; dx<2; ++dx)
				{
					const int32_t tgx = (x*2)+dx;

					c = *(uint32_t*)(&src[(tgx*4)+(tgy*pitchsrc)]);

					a += c.getAlpha();
					r += c.getRed();
					g += c.getGreen();
					b += c.getBlue();
				}
			}

			a /= 4;
			r /= 4;
			g /= 4;
			b /= 4;

			c.set(a, r, g, b);
			*(uint32_t*)(&tgt[(x*4)+(y*pitchtgt)]) = c.color;
		}
	}
}