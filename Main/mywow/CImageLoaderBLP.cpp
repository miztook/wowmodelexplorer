#include "stdafx.h"
#include "CImageLoaderBLP.h"
#include "mywow.h"
#include "ddslib.h"
#include "CImage.h"
#include "CBLPImage.h"

IImage* CImageLoaderBLP::loadAsImage( IMemFile* file, bool changeRB)
{
	uint8_t* buffer = (uint8_t*)file->getBuffer();

	CBLPImage::SBLPHeader* header = reinterpret_cast<CBLPImage::SBLPHeader*>(buffer);

	if (header->magic != FOURCC('B', 'L', 'P', '2'))
	{
		ASSERT(false);
		return 0;
	}

	ASSERT(header->_version == 1);
	bool isABGR = !g_Engine->isDXFamily();

	uint32_t width = header->_xres;
	uint32_t height = header->_yres;
	uint32_t size = width * height;

	uint8_t* src = buffer + header->_mipmapOfs[0];

	dimension2du dim(width, height);

	uint32_t* decompressed = new uint32_t[size];
	if (header->_compress == 2)				//compressed
	{
		if (header->_alphaCompress == 7)			//dxt5
		{
			DDSDecompressDXT5(src, width, height, (uint8_t*)decompressed);
		}
		else if (header->_alphaCompress == 1)				//dxt3
		{
			DDSDecompressDXT3(src, width, height, (uint8_t*)decompressed);
		}	
		else if (header->_alphaCompress == 0)
		{
			DDSDecompressDXT1(src, width, height, (uint8_t*)decompressed, true);
		}
		else
		{
			ASSERT(false);
		}
	}
	else
	{
		uint32_t* palette = (uint32_t*)(buffer + sizeof(CBLPImage::SBLPHeader));
		SColor c;
		for (uint32_t i=0; i<width*height; ++i)
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
					uint32_t a = (src[(size + i/8)] >> (i%8)) & 1;
					c.setAlpha( a ? 0xff : 0);
				}
				break;
			case 4:
				{
					c = palette[src[i]];
					uint32_t a;
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
		for (uint32_t h=0; h<dim.Height; ++h)
		{
			for (uint32_t w=0; w<dim.Width; ++w)
			{
				SColor c = decompressed[h*dim.Width + w];
				decompressed[h*dim.Width + w] = SColor(c.getAlpha(), c.getBlue(), c.getGreen(), c.getRed()).color;
			}
		}
	}
	
	CImage* image = new CImage(ECF_A8R8G8B8, dim, decompressed, true);
	return image;
}
