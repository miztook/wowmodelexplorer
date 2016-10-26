#include "stdafx.h"
#include "CImageLoaderPNG.h"
#include "mywow.h"
#include "CImage.h"
#include "CBlit.h"
#include "png.h"

static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
	CSysUtility::outputDebug("PNG fatal error", msg);
	longjmp(png_jmpbuf(png_ptr), 1);
}

static void png_cpexcept_warn(png_structp png_ptr, png_const_charp msg)
{
	CSysUtility::outputDebug("PNG warning", msg);
}

void PNGAPI user_read_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	// changed by zola {
	IMemFile* file=(IMemFile*)png_get_io_ptr(png_ptr);
	u32 dwRead = file->read((void*)data, (u32)length);
	// }

	if (dwRead != (u32)length)
		png_error(png_ptr, "Read Error");
}

IImage* CImageLoaderPNG::loadAsImage( IMemFile* file )
{
	u8** RowPointers = NULL_PTR;

	png_byte buffer[8];
	if(file->read(buffer, 8) != 8)
	{
		CSysUtility::outputDebug("LOAD PNG: can't read file", file->getFileName());
		return NULL_PTR;
	}

	if( png_sig_cmp(buffer, 0, 8) )
	{
		CSysUtility::outputDebug("LOAD PNG: not really a png", file->getFileName());
		return NULL_PTR;
	}

	// Allocate the png read struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL_PTR, (png_error_ptr)png_cpexcept_error, (png_error_ptr)png_cpexcept_warn);
	if (!png_ptr)
	{
		CSysUtility::outputDebug("LOAD PNG: Internal PNG create read struct failure", file->getFileName());
		return 0;
	}

	// Allocate the png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		CSysUtility::outputDebug("LOAD PNG: Internal PNG create info struct failure", file->getFileName());
		png_destroy_read_struct(&png_ptr, NULL_PTR, NULL_PTR);
		return 0;
	}

	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL_PTR);
		if (RowPointers)
			delete [] RowPointers;
		return 0;
	}

	// changed by zola so we don't need to have public FILE pointers
	png_set_read_fn(png_ptr, file, user_read_data_fcn);

	png_set_sig_bytes(png_ptr, 8); // Tell png that we read the signature

	png_read_info(png_ptr, info_ptr); // Read the info section of the png file

	u32 Width;
	u32 Height;
	int BitDepth;
	int ColorType;
	{
		// Use temporary variables to avoid passing casted pointers
		png_uint_32 w,h;
		// Extract info
		png_get_IHDR(png_ptr, info_ptr,
			&w, &h,
			&BitDepth, &ColorType, NULL_PTR, NULL_PTR, NULL_PTR);
		Width=w;
		Height=h;
	}

	// Convert palette color to true color
	if (ColorType==PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// Convert low bit colors to 8 bit colors
	if (BitDepth < 8)
	{
		if (ColorType==PNG_COLOR_TYPE_GRAY || ColorType==PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		else
			png_set_packing(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// Convert high bit colors to 8 bit colors
	if (BitDepth == 16)
		png_set_strip_16(png_ptr);

	// Convert gray color to true color
	if (ColorType==PNG_COLOR_TYPE_GRAY || ColorType==PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	int intent;
	const double screen_gamma = 2.2;

	if (png_get_sRGB(png_ptr, info_ptr, &intent))
		png_set_gamma(png_ptr, screen_gamma, 0.45455);
	else
	{
		double image_gamma;
		if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
			png_set_gamma(png_ptr, screen_gamma, image_gamma);
		else
			png_set_gamma(png_ptr, screen_gamma, 0.45455);
	}

	// Update the changes in between, as we need to get the new color type
	// for proper processing of the RGBA type
	png_read_update_info(png_ptr, info_ptr);
	{
		// Use temporary variables to avoid passing casted pointers
		png_uint_32 w,h;
		// Extract info
		png_get_IHDR(png_ptr, info_ptr,
			&w, &h,
			&BitDepth, &ColorType, NULL_PTR, NULL_PTR, NULL_PTR);
		Width=w;
		Height=h;
	}

	// Convert RGBA to BGRA
	if (ColorType==PNG_COLOR_TYPE_RGB_ALPHA)
	{
		png_set_bgr(png_ptr);
	}

	u32 dataSize = 0;
	u8* pImageData = NULL_PTR;
	IImage* image = new CImage(ECF_A8R8G8B8, dimension2du(Width, Height));
	u32 pitch = 0;

	if (ColorType==PNG_COLOR_TYPE_RGB_ALPHA)
	{
		pitch = getBytesPerPixelFromFormat(ECF_A8R8G8B8) * Width;
		dataSize =  pitch * Height;
	}
	else
	{
		pitch = getBytesPerPixelFromFormat(ECF_R8G8B8) * Width;
		dataSize =  pitch * Height;
	}

	pImageData = new u8[dataSize];

	// Create array of pointers to rows in image data
	RowPointers = new png_bytep[Height];

	// Fill array of pointers to rows in image data
	unsigned char* data = (unsigned char*)pImageData;
	for (u32 i=0; i<Height; ++i)
	{
		RowPointers[i]=data;
		data += pitch;
	}

	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL_PTR);
		delete [] RowPointers;
		image->drop();
		return NULL_PTR;
	}

	// Read data using the library function that handles all transformations including interlacing
	png_read_image(png_ptr, RowPointers);

	if (ColorType==PNG_COLOR_TYPE_RGB_ALPHA)
	{
		bool argb = g_Engine->isDXFamily();
		CBlit::convert32BitTo32Bit((s32*)pImageData, (s32*)image->lock(), Width, Height, 0, false, !argb);
	}
	else
	{
		bool argb = g_Engine->isDXFamily();
		u8* src = pImageData;
		u8* target = (u8*)image->lock();
		for (u32 i=0; i<Height; ++i)
		{
			for(u32 k=0; k<Width; ++k)
			{
				u32 idx = k + i * Width;

				const u8* srcPixel = src + 3 * idx;
				u8* dstPixel = target + 4 * idx;

				if(argb)
				{
					dstPixel[0] = srcPixel[2];		//b
					dstPixel[1] = srcPixel[1];		//g
					dstPixel[2] = srcPixel[0];		//r
				}
				else
				{
					dstPixel[0] = srcPixel[0];		//r
					dstPixel[1] = srcPixel[1];		//g
					dstPixel[2] = srcPixel[2];		//b
				}
				dstPixel[3] = 0xff;
			}
		}
	}

	png_read_end(png_ptr, NULL_PTR);
	delete [] RowPointers;
	png_destroy_read_struct(&png_ptr,&info_ptr, 0); // Clean up memory

	delete[] pImageData;

	return image;
}