#include "stdafx.h"
#include "CBLPImage.h"
#include "mywow.h"
#include "CBlit.h"
#include "ddslib.h"

CBLPImage::CBLPImage()
{
	FileData = nullptr;
}

CBLPImage::~CBLPImage()
{
	delete[] FileData;
}

bool CBLPImage::loadFile( IMemFile* file, bool abgr )
{
	IsABGR = abgr;

	uint32_t filesize = file->getSize();
	FileData = new uint8_t[filesize];
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
	for (uint32_t i=0; i<16; ++i)
	{
		if(!header->_mipmapOfs[i])
			break;
		++NumMipMaps;
	}

	//check mipsize
	for (uint32_t i=0; i<NumMipMaps; ++i)
	{
		dimension2du mipsize = Size.getMipLevelSize(i);
		uint32_t bytes;
		getImagePitchAndBytes(Format, mipsize.Width, mipsize.Height, MipmapPitch[i], bytes);

// 		if(Format != ECF_A8R8G8B8)			//compressed
// 			ASSERT(header->_mipmapSize[i] == bytes);
	}

	return true;
}


bool CBLPImage::fromImageData( const uint8_t* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap )
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

	uint32_t offset = sizeof(SBLPHeader);
	for (uint32_t i=0; i<NumMipMaps; ++i)
	{
		dimension2du mipsize = Size.getMipLevelSize(i);
		header._mipmapOfs[i] = offset;

		uint32_t bytes;
		getImagePitchAndBytes(Format, mipsize.Width, mipsize.Height, MipmapPitch[i], bytes);

		header._mipmapSize[i] = bytes;
		offset += header._mipmapSize[i];
	}

	Q_memcpy(MipmapDataSize, sizeof(MipmapDataSize), header._mipmapSize, sizeof(header._mipmapSize));

	FileData = new uint8_t[offset];
	Q_memcpy(FileData, sizeof(SBLPHeader), &header, sizeof(SBLPHeader));

	//
	uint32_t mipmapOfs[16] = {0};
	offset = 0;
	uint32_t bpp = getBytesPerPixelFromFormat(ECF_R5G6B5);
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
	CBlit::resizeBilinearA8R8G8B8(src, Size.Width, Size.Height, tmpdata, Size.Width, Size.Height, ECF_R5G6B5);
	//CBlit::resizeBicubicA8R8G8B8(src, Size.Width, Size.Height, tmpdata, Size.Width, Size.Height, ECF_R5G6B5);

	//mip 0 data -> dxt1
	uint8_t* dst0 = FileData + header._mipmapOfs[0];
	if (header._mipmapSize[0] != DDSCompressRGB565ToDXT1(tmpdata, Size.Width, Size.Height, dst0))
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
		copy16BitMipMap(s, tgt, lower.Width, lower.Height, bpp * upper.Width, bpp * lower.Width);

		//mip i data -> dxt1
		uint8_t* dst = FileData + header._mipmapOfs[i];
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

const void* CBLPImage::getMipmapData( uint32_t level ) const
{
	if (level >= 16)
		return nullptr;

	SBLPHeader* header = reinterpret_cast<SBLPHeader*>(FileData);
	if (header->_mipmapSize[level])
		return FileData + header->_mipmapOfs[level];

	return nullptr;
}

bool CBLPImage::copyMipmapData( uint32_t level, void* dest, uint32_t pitch, uint32_t width, uint32_t height )
{
	uint32_t limit = getMipmapDataSize(level);
	const uint8_t* src = static_cast<const uint8_t*>(getMipmapData(level));
	if (!src || !limit )
	{
		ASSERT(false);
		return false;
	}

	if (Format != ECF_A8R8G8B8)
	{
		uint32_t xblock = (width + 3) / 4;
		uint32_t yblock = (height + 3) / 4;

		uint8_t* target = (uint8_t*)dest;
		uint32_t blocksize = getBytesPerPixelFromFormat(Format);

		ASSERT(blocksize * xblock >= pitch);

		const uint8_t* p = src;
		for (uint32_t i = 0; i < yblock; ++i)
		{
			Q_memcpy(target, pitch, p, xblock * blocksize);
			target += pitch;

			if (p < src + limit)
				p += xblock * blocksize;
		}
	}
	else
	{
		uint32_t* target = (uint32_t*)dest;
		uint32_t* palette = (uint32_t*)(FileData + sizeof(SBLPHeader));
		uint32_t	size = width * height;
		SColor c;
		for (uint32_t i=0; i<height; ++i)
		{
			for(uint32_t k=0; k<width; ++k)
			{
				uint32_t idx = k + width * i;
			
				switch (AlphaDepth)
				{
				case 0:
					c = palette[src[idx]];
					c.setAlpha(0xff);
					break;
				case 1:
					{
						c = palette[src[idx]];
						uint32_t a = (src[(size + idx/8)] >> (idx%8)) & 1;
						c.setAlpha( a ? 0xff : 0);
					}
					break;
				case 4:
					{
						c = palette[src[idx]];
						uint32_t a;
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
			target += (pitch / sizeof(uint32_t));
		}
	}

	return true;
}

void CBLPImage::copy16BitMipMap( const uint8_t* src, uint8_t* tgt, uint32_t width, uint32_t height, uint32_t pitchsrc, uint32_t pitchtgt ) const
{
	for (uint32_t y=0; y<height; ++y)
	{
		for (uint32_t x=0; x<width; ++x)
		{
			uint32_t a=0, r=0, g=0, b=0;

			for (int32_t dy=0; dy<2; ++dy)
			{
				const int32_t tgy = (y*2)+dy;
				for (int32_t dx=0; dx<2; ++dx)
				{
					const int32_t tgx = (x*2)+dx;

					SColor c = SColor::R5G6B5toA8R8G8B8(*(uint16_t*)(&src[(tgx*2)+(tgy*pitchsrc)]));

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

			uint16_t c;
			c = SColor::A8R8G8B8toR5G6B5(SColor(a,r,g,b).color);
			*(uint16_t*)(&tgt[(x*2)+(y*pitchtgt)]) = c;
		}
	}
}
