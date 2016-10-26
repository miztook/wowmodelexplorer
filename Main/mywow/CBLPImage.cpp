#include "stdafx.h"
#include "CBLPImage.h"
#include "mywow.h"
#include "CBlit.h"
#include "ddslib.h"

CBLPImage::CBLPImage()
{
	FileData = NULL_PTR;
}

CBLPImage::~CBLPImage()
{
	delete[] FileData;
}

bool CBLPImage::loadFile( IMemFile* file, bool abgr )
{
	IsABGR = abgr;

	u32 filesize = file->getSize();
	FileData = new u8[filesize];
	Q_memcpy(FileData, filesize, file->getBuffer(), filesize);

	SBLPHeader* header = reinterpret_cast<SBLPHeader*>(FileData);

	if (header->magic != FOURCC('B', 'L', 'P', '2'))
	{
		ASSERT(false);
		return false;
	}

	ASSERT(header->_version == 1);

	Size.Width = header->_xres;
	Size.Height = header->_yres;
	AlphaDepth = header->_alphaDepth;

	Q_memcpy(MipmapDataSize, sizeof(MipmapDataSize), header->_mipmapSize, sizeof(header->_mipmapSize));

	if (header->_compress == 2)				//compressed
	{
		if (header->_alphaCompress == 7)			//dxt5
		{
			Format = ECF_DXT5;
		}
		else if (header->_alphaCompress == 1)				//dxt3
		{
			Format = ECF_DXT3;
		}	
		else if (header->_alphaCompress == 0)
		{
			Format = ECF_DXT1;
		}
		else
		{
			ASSERT(false);
		}
	}
	else
	{
		Format = ECF_A8R8G8B8;
	}

	NumMipMaps = 0;
	for (u32 i=0; i<16; ++i)
	{
		if(!header->_mipmapOfs[i])
			break;
		++NumMipMaps;
	}

	//check mipsize
	for (u32 i=0; i<NumMipMaps; ++i)
	{
		dimension2du mipsize = Size.getMipLevelSize(i);
		u32 bytes;
		getImagePitchAndBytes(Format, mipsize.Width, mipsize.Height, MipmapPitch[i], bytes);

// 		if(Format != ECF_A8R8G8B8)			//compressed
// 			ASSERT(header->_mipmapSize[i] == bytes);
	}

	return true;
}


bool CBLPImage::fromImageData( const u8* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap )
{
	if (size.Width % 4 != 0 || size.Height % 4 != 0 || format != ECF_A8R8G8B8)
	{
		ASSERT(false);
		return false;
	}

	Format = ECF_DXT1;
	AlphaDepth = 0;
	Size = size;

	SBLPHeader header = {0};
	header.magic = FOURCC('B', 'L', 'P', '2');
	header._version = 1;
	header._compress = 2;
	header._alphaDepth = 0;
	header._alphaCompress = 0;
	header._mipmap = 1;
	header._xres = Size.Width;
	header._yres = Size.Height;

	NumMipMaps = mipmap ? Size.getNumMipLevels() : 1;
	if (NumMipMaps > 16)
	{
		ASSERT(false);
		return false;
	}

	u32 offset = sizeof(SBLPHeader);
	for (u32 i=0; i<NumMipMaps; ++i)
	{
		dimension2du mipsize = Size.getMipLevelSize(i);
		header._mipmapOfs[i] = offset;

		u32 bytes;
		getImagePitchAndBytes(Format, mipsize.Width, mipsize.Height, MipmapPitch[i], bytes);

		header._mipmapSize[i] = bytes;
		offset += header._mipmapSize[i];
	}

	Q_memcpy(MipmapDataSize, sizeof(MipmapDataSize), header._mipmapSize, sizeof(header._mipmapSize));

	FileData = new u8[offset];
	Q_memcpy(FileData, sizeof(SBLPHeader), &header, sizeof(SBLPHeader));

	//
	u32 mipmapOfs[16] = {0};
	offset = 0;
	u32 bpp = getBytesPerPixelFromFormat(ECF_R5G6B5);
	for (u32 i=0; i<NumMipMaps; ++i)
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
    
	u8* tmpdata = (u8*)Z_AllocateTempMemory(offset);
	
	//mip 0 data
	CBlit::resizeBilinearA8R8G8B8(src, Size.Width, Size.Height, tmpdata, Size.Width, Size.Height, ECF_R5G6B5);
	//CBlit::resizeBicubicA8R8G8B8(src, Size.Width, Size.Height, tmpdata, Size.Width, Size.Height, ECF_R5G6B5);

	//mip 0 data -> dxt1
	u8* dst0 = FileData + header._mipmapOfs[0];
	if (header._mipmapSize[0] != DDSCompressRGB565ToDXT1(tmpdata, Size.Width, Size.Height, dst0))
	{
		ASSERT(false);
		Z_FreeTempMemory(tmpdata);
		return false;
	}

	for (u32 i=1; i<NumMipMaps; ++i)
	{
		dimension2du upper = Size.getMipLevelSize(i-1);
		dimension2du lower = Size.getMipLevelSize(i);
		const u8* s = tmpdata + mipmapOfs[i-1];
		u8* tgt = tmpdata + mipmapOfs[i];

		//mip i data
		copy16BitMipMap(s, tgt, lower.Width, lower.Height, bpp * upper.Width, bpp * lower.Width);

		//mip i data -> dxt1
		u8* dst = FileData + header._mipmapOfs[i];
		if (header._mipmapSize[i] != DDSCompressRGB565ToDXT1(tgt, lower.Width, lower.Height, dst))
		{
			ASSERT(false);
			Z_FreeTempMemory(tmpdata);
			return false;
		}
	}

	Z_FreeTempMemory(tmpdata);

	return true;
}

const void* CBLPImage::getMipmapData( u32 level ) const
{
	if (level >= 16)
		return NULL_PTR;

	SBLPHeader* header = reinterpret_cast<SBLPHeader*>(FileData);
	if (header->_mipmapSize[level])
		return FileData + header->_mipmapOfs[level];

	return NULL_PTR;
}

bool CBLPImage::copyMipmapData( u32 level, void* dest, u32 pitch, u32 width, u32 height )
{
	u32 limit = getMipmapDataSize(level);
	const u8* src = static_cast<const u8*>(getMipmapData(level));
	if (!src || !limit )
	{
		ASSERT(false);
		return false;
	}

	if (Format != ECF_A8R8G8B8)
	{
		u32 xblock = (width + 3) / 4;
		u32 yblock = (height + 3) / 4;

		u8* target = (u8*)dest;
		u32 blocksize = getBytesPerPixelFromFormat(Format);

		ASSERT(blocksize * xblock >= pitch);

		const u8* p = src;
		for (u32 i = 0; i < yblock; ++i)
		{
			Q_memcpy(target, pitch, p, xblock * blocksize);
			target += pitch;

			if (p < src + limit)
				p += xblock * blocksize;
		}
	}
	else
	{
		u32* target = (u32*)dest;
		u32* palette = (u32*)(FileData + sizeof(SBLPHeader));
		u32	size = width * height;
		SColor c;
		for (u32 i=0; i<height; ++i)
		{
			for(u32 k=0; k<width; ++k)
			{
				u32 idx = k + width * i;
			
				switch (AlphaDepth)
				{
				case 0:
					c = palette[src[idx]];
					c.setAlpha(0xff);
					break;
				case 1:
					{
						c = palette[src[idx]];
						u32 a = (src[(size + idx/8)] >> (idx%8)) & 1;
						c.setAlpha( a ? 0xff : 0);
					}
					break;
				case 4:
					{
						c = palette[src[idx]];
						u32 a;
						if (i%2)
							a = (src[(size+idx/2)] >> 4) & 0x000f;
						else
							a = (src[(size+idx/2)] & 0x000f);

						a = a << 4;
						c.setAlpha( a );
					}
					break;
				case 8:
					c = palette[src[idx]];
					c.setAlpha(src[(size + idx)]);
					break;
				}

				if (IsABGR)
					c.set(c.getAlpha(), c.getBlue(), c.getGreen(), c.getRed());
				target[k] = c.color;
			}
			target += (pitch / sizeof(u32));
		}
	}

	return true;
}

void CBLPImage::copy16BitMipMap( const u8* src, u8* tgt, u32 width, u32 height, u32 pitchsrc, u32 pitchtgt ) const
{
	for (u32 y=0; y<height; ++y)
	{
		for (u32 x=0; x<width; ++x)
		{
			u32 a=0, r=0, g=0, b=0;

			for (s32 dy=0; dy<2; ++dy)
			{
				const s32 tgy = (y*2)+dy;
				for (s32 dx=0; dx<2; ++dx)
				{
					const s32 tgx = (x*2)+dx;

					SColor c = SColor::R5G6B5toA8R8G8B8(*(u16*)(&src[(tgx*2)+(tgy*pitchsrc)]));

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

			u16 c;
			c = SColor::A8R8G8B8toR5G6B5(SColor(a,r,g,b).color);
			*(u16*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
	}
}
