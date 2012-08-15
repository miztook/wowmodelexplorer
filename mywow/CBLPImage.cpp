#include "stdafx.h"
#include "CBLPImage.h"
#include "mpq_libmpq.h"

CBLPImage::CBLPImage()
{
	FileData = NULL;
	AlphaDepth = 0;
}

CBLPImage::~CBLPImage()
{
	delete FileData;
}

bool CBLPImage::loadFile( MPQFile* file )
{
	u32 filesize = file->getSize();
	FileData = new u8[filesize];
	memcpy_s(FileData, filesize, file->getBuffer(), filesize);

	SBLPHeader* header = (SBLPHeader*)FileData;

	if (header->_magic[0] != 'B' ||
		header->_magic[1] != 'L' ||
		header->_magic[2] != 'P' ||
		header->_magic[3] != '2')
	{
		_ASSERT(false);
		return false;
	}

	_ASSERT(header->_version == 1);

	Size.Width = header->_xres;
	Size.Height = header->_yres;
	AlphaDepth = header->_alphaDepth;

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
			_ASSERT(false);
		}
	}
	else
	{
		Format = ECF_A8R8G8B8;
	}

	return true;
}

void* CBLPImage::getMipmapData( u32 level ) const
{
	if (level >= 16)
		return NULL;

	SBLPHeader* header = (SBLPHeader*)FileData;
	if (header->_mipmapSize[level])
		return FileData + header->_mipmapOfs[level];

	return NULL;
}

void CBLPImage::copyMipmapData( u32 level, void* dest, u32 pitch, u32 width, u32 height )
{
	if (Format != ECF_A8R8G8B8)
	{
		u32 xblock = (width + 3) / 4;
		u32 yblock = (height + 3) / 4;

		u8* src = (u8*)getMipmapData(level);
		u8* target = (u8*)dest;
		u32 blocksize = getBytesPerPixelFromFormat(Format);
		for (u32 i = 0; i < yblock; ++i)
		{
			memcpy_s(target, pitch, src, xblock * blocksize);
			target += pitch;
			src += xblock * blocksize;
		}
	}
	else
	{
		u8* src = (u8*)getMipmapData(level);
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
				target[k] = c.color;
			}
			target += (pitch / sizeof(u32));
		}
	}

}
