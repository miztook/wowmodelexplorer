#include "stdafx.h"
#include "CImageLoaderBLP.h"
#include "mywow.h"
#include "ddslib.h"
#include "CImage.h"
#include "CBLPImage.h"

IImage* CImageLoaderBLP::loadAsImage( IMemFile* file, bool changeRB)
{
	u8* buffer = (u8*)file->getBuffer();

	CBLPImage::SBLPHeader* header = reinterpret_cast<CBLPImage::SBLPHeader*>(buffer);

	if (header->magic != FOURCC('B', 'L', 'P', '2'))
	{
		ASSERT(false);
		return 0;
	}

	ASSERT(header->_version == 1);
	bool isABGR = !g_Engine->isDXFamily();

	u32 width = header->_xres;
	u32 height = header->_yres;
	u32 size = width * height;

	u8* src = buffer + header->_mipmapOfs[0];

	dimension2du dim(width, height);

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
			ASSERT(false);
		}
	}
	else
	{
		u32* palette = (u32*)(buffer + sizeof(CBLPImage::SBLPHeader));
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

	if (changeRB)
	{
		for (u32 h=0; h<dim.Height; ++h)
		{
			for (u32 w=0; w<dim.Width; ++w)
			{
				SColor c = decompressed[h*dim.Width + w];
				decompressed[h*dim.Width + w] = SColor(c.getAlpha(), c.getBlue(), c.getGreen(), c.getRed()).color;
			}
		}
	}
	
	CImage* image = new CImage(ECF_A8R8G8B8, dim, decompressed, true);
	return image;
}
