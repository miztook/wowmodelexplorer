#include "stdafx.h"
#include "CImageLoaderBLP.h"
#include "mpq_libmpq.h"
#include "mywow.h"
#include "ddslib.h"
#include "CImage.h"
#include "CBlit.h"

IImage* CImageLoaderBLP::loadImage( MPQFile* file )
{
	u8* buffer = (u8*)file->getBuffer();

	SBLPHeader* header = (SBLPHeader*)buffer;

	if (header->_magic[0] != 'B' ||
		header->_magic[1] != 'L' ||
		header->_magic[2] != 'P' ||
		header->_magic[3] != '2')
	{
		_ASSERT(false);
		return 0;
	}

	_ASSERT(header->_version == 1);

	u32 width = header->_xres;
	u32 height = header->_yres;
	u32 size = width * height;

	u8* src = buffer + header->_mipmapOfs[0];

	u32* decompressed = new u32[size];
	if (header->_compress == 2)				//compressed
	{
		if (header->_alphaCompress == 7)			//dxt5
		{
			DDSDecompressDXT5(src, width, height, (u8*)decompressed);
		}
		else if (header->_alphaCompress == 1)				//dxt3
		{
			DDSDecompressDXT3(src, width, height, (u8*)decompressed);
		}	
		else if (header->_alphaCompress == 0)
		{
			DDSDecompressDXT1(src, width, height, (u8*)decompressed, true);
		}
		else
		{
			DDSDecompressDXT1(src, width, height, (u8*)decompressed, false);
		}
	}
	else
	{
		u32* palette = (u32*)(buffer + sizeof(SBLPHeader));
		SColor c;
		for (u32 i=0; i<width*height; ++i)
		{
			switch (header->_alphaDepth)
			{
			case 0:
				c = palette[src[i]];
				c.setAlpha(0xff);
				break;
			case 1:
				{
					c = palette[src[i]];
					u32 a = (src[(size + i/8)] >> (i%8)) & 1;
					c.setAlpha( a ? 0xff : 0);
				}
				break;
			case 4:
				{
					c = palette[src[i]];
					u32 a;
					if (i%2)
						a = (src[(size+i/2)] >> 4) & 0x000f;
					else
						a = (src[(size+i/2)] & 0x000f);

					a = a << 4;
					c.setAlpha( a );
				}
				break;
			case 8:
				c = palette[src[i]];
				c.setAlpha(src[(size + i)]);
				break;
			}
			decompressed[i] = c.color;
		}
	}

	dimension2du dim(width, height);
	CImage* image = new CImage(ECF_A8R8G8B8, dim, decompressed, true);
	return image;
}

