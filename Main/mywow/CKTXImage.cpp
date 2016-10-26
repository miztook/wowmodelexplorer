#include "stdafx.h"
#include "CKTXImage.h"
#include "mywow.h"
#include "CBlit.h"

// KTX Header Identifiers.
const u8 KTX_IDENTIFIER[12] = {0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

CKTXImage::CKTXImage()
{
	FileData = NULL_PTR;
	memset(MipmapOffset, 0, sizeof(MipmapOffset));
}

CKTXImage::~CKTXImage()
{
	delete[] FileData;
}

bool CKTXImage::loadFile( IMemFile* file )
{
	u32 filesize = file->getSize();
	FileData = new u8[filesize];
	Q_memcpy(FileData, filesize, file->getBuffer(), filesize);

	u32 offset = 0;
	KTX_Header header;
	Q_memcpy(&header, sizeof(KTX_Header), FileData, sizeof(KTX_Header));
	offset = sizeof(KTX_Header);

	if (memcmp(header.identifier, KTX_IDENTIFIER, 12) != 0)
	{
		ASSERT(false);
		return false;
	}

	Size.Width = header.pixelWidth;
	Size.Height = header.pixelHeight;
	NumMipMaps = header.numberOfMipmapLevels;

	switch(header.glInternalFormat)
	{
	case GL_ETC1_RGB8_OES:
		{
			Format = ECF_ETC1_RGB;

			//metadata
			if (header.bytesOfKeyValueData == sizeof(KTX_Metadata))
			{
				KTX_Metadata metadata;
				Q_memcpy(&metadata, sizeof(KTX_Metadata), &FileData[offset], sizeof(KTX_Metadata));
				offset += sizeof(KTX_Metadata);

				if (metadata.key == KTX_METADATA_ALPHAINFO && metadata.data == 1)
					Format = ECF_ETC1_RGBA;
			}
			else if (header.bytesOfKeyValueData > 0)
			{
				offset += header.bytesOfKeyValueData;
			}
		}
		break;
	case GL_RGB8:
		{
			Format = ECF_R8G8B8;
			if (header.bytesOfKeyValueData > 0)
			{
				offset += header.bytesOfKeyValueData;
			}
		}
		break;
	case GL_RGBA8:
		{
			Format = ECF_A8R8G8B8;
			if (header.bytesOfKeyValueData > 0)
			{
				offset += header.bytesOfKeyValueData;
			}
		}
		break;
	case GL_ATC_RGB_AMD:
		{
			Format = ECF_ATC_RGB;
			if (header.bytesOfKeyValueData > 0)
			{
				offset += header.bytesOfKeyValueData;
			}
		}
		break;
	case GL_ATC_RGBA_EXPLICIT_ALPHA_AMD:
		{
			Format = ECF_ATC_RGBA_EXPLICIT;
			if (header.bytesOfKeyValueData > 0)
			{
				offset += header.bytesOfKeyValueData;
			}
		}
		break;
	case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
		{
			Format = ECF_ATC_RGBA_INTERPOLATED;
			if (header.bytesOfKeyValueData > 0)
			{
				offset += header.bytesOfKeyValueData;
			}
		}
		break;
	default:
		{
			ASSERT(false);			//not supported format
			return false;
		}
	}

	//except etc1_rgba
	if (isCompressedWithAlphaFormat(Format))
	{
		if (!isPOT(Size.Width) || !isPOT(Size.Height))
		{
			ASSERT(false);
			return false;
		}
	}
	else
	{
		if (isCompressedFormat(Format) &&
			(Size.Width != Size.Height || !isPOT(Size.Width)))
		{
			ASSERT(false);
			return false;
		}
	}

	u32 mipoffset = offset;
	for (u32 i=0; i<NumMipMaps; ++i)
	{
		mipoffset += sizeof(u32);		//not use
		MipmapOffset[i] = mipoffset;

		dimension2du mipsize = Size.getMipLevelSize(i);
		getImagePitchAndBytes(Format, mipsize.Width, mipsize.Height, MipmapPitch[i], MipmapDataSize[i]);

		mipoffset += ((MipmapDataSize[i] + 3) & ~3);
	}

	ASSERT(mipoffset == filesize);

	return true;
}

bool CKTXImage::fromImageData( const u8* src, const dimension2du& size, ECOLOR_FORMAT format, bool mipmap )
{
	ASSERT(false);
	return false;
}

const void* CKTXImage::getMipmapData( u32 level ) const
{
	if (level >= 16)
		return NULL_PTR;

	if (MipmapDataSize[level])
		return FileData +MipmapOffset[level];

	return NULL_PTR;
}

bool CKTXImage::copyMipmapData( u32 level, void* dest, u32 pitch, u32 width, u32 height )
{
	u32 limit = getMipmapDataSize(level);
	const u8* src = static_cast<const u8*>(getMipmapData(level));
	if (!src || !limit )
	{
		ASSERT(false);
		return false;
	}

	u32 srcPitch, srcDataSize;
	getImagePitchAndBytes(Format, width, height, srcPitch, srcDataSize);
	if (srcPitch != pitch)
	{
		ASSERT(false);
		return false;
	}

	Q_memcpy(dest, srcDataSize, src, srcDataSize);

	return true;
}
