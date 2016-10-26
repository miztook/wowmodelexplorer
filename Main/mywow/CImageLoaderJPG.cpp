#include "stdafx.h"
#include "CImageLoaderJPG.h"
#include "mywow.h"
#include "CImage.h"

IImage* CImageLoaderJPG::loadAsImage( IMemFile* file )
{
	bool revert = !g_Engine->isDXFamily();

	u8* buffer = (u8*)file->getBuffer();

	struct jpeg_decompress_struct cinfo;
	struct irr_jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	cinfo.err->error_exit = error_exit;
	cinfo.err->output_message = output_message;

	jpeg_create_decompress(&cinfo);

	// specify data source
	jpeg_source_mgr jsrc;

	// Set up data pointer
	jsrc.bytes_in_buffer = file->getSize();
	jsrc.next_input_byte = (JOCTET*)buffer;
	cinfo.src = &jsrc;

	jsrc.init_source = init_source;
	jsrc.fill_input_buffer = fill_input_buffer;
	jsrc.skip_input_data = skip_input_data;
	jsrc.resync_to_restart = jpeg_resync_to_restart;
	jsrc.term_source = term_source;

	jpeg_read_header(&cinfo, TRUE);

	bool useCMYK;
	if (cinfo.jpeg_color_space==JCS_CMYK)
	{
		cinfo.out_color_space=JCS_CMYK;
		cinfo.out_color_components=4;
		useCMYK = true;
	}
	else
	{
		cinfo.out_color_space=JCS_RGB;
		cinfo.out_color_components=3;
		useCMYK = false;
	}
	cinfo.output_gamma=2.2;
	cinfo.do_fancy_upsampling=FALSE;

	// Start decompressor
	jpeg_start_decompress(&cinfo);

	// Get image data
	u16 rowspan = cinfo.image_width * cinfo.out_color_components;
	u32 width = cinfo.image_width;
	u32 height = cinfo.image_height;

	// Allocate memory for buffer
	u8* output = new u8[rowspan * height];

	u8** rowPtr = (u8**)Z_AllocateTempMemory(sizeof(u8*) * height);

	for( u32 i = 0; i < height; i++ )
		rowPtr[i] = &output[ i * rowspan ];

	u32 rowsRead = 0;
	while( cinfo.output_scanline < cinfo.output_height )
		rowsRead += jpeg_read_scanlines( &cinfo, &rowPtr[rowsRead], cinfo.output_height - rowsRead );

	Z_FreeTempMemory(rowPtr);

	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	IImage* image = 0;
	if (useCMYK)
	{
		if (revert)
		{
			u32 size = 4*width*height;
			u8* data = new u8[size];
			for (u32 i=0,j=0; i<size; i+=4, j+=4)
			{
				data[i+0] = (u8)(output[j+2]);
				data[i+1] = (u8)(output[j+1]);
				data[i+2] = (u8)(output[j+0]);
				data[i+3] = (u8)(output[j+3]);
			}
			delete[] output;
			image = new CImage(ECF_A8R8G8B8, dimension2d<u32>(width, height), data, true);
		}
		else
		{
			image = new CImage(ECF_A8R8G8B8, dimension2d<u32>(width, height), output, true);
		}
	}
	else
	{
		u32 size = 4*width*height;
		u8* data = new u8[size];

		if (revert)
		{
			for (u32 i=0,j=0; i<size; i+=4, j+=3)
			{
				data[i+0] = (u8)(output[j+0]);
				data[i+1] = (u8)(output[j+1]);
				data[i+2] = (u8)(output[j+2]);
				data[i+3] = 255;
			}
		}
		else
		{
			for (u32 i=0,j=0; i<size; i+=4, j+=3)
			{
				data[i+0] = (u8)(output[j+2]);
				data[i+1] = (u8)(output[j+1]);
				data[i+2] = (u8)(output[j+0]);
				data[i+3] = 255;
			}
		}
		delete[] output;

		image = new CImage(ECF_A8R8G8B8, dimension2d<u32>(width, height), data, true);
	}

	return image;
}

void CImageLoaderJPG::init_source( j_decompress_ptr cinfo )
{

}

boolean CImageLoaderJPG::fill_input_buffer( j_decompress_ptr cinfo )
{
	return true;
}

void CImageLoaderJPG::skip_input_data( j_decompress_ptr cinfo, long num_bytes )
{
	jpeg_source_mgr * src = cinfo->src;
	if(num_bytes > 0)
	{
		src->bytes_in_buffer -= num_bytes;
		src->next_input_byte += num_bytes;
	}
}

void CImageLoaderJPG::term_source( j_decompress_ptr cinfo )
{

}

void CImageLoaderJPG::error_exit( j_common_ptr cinfo )
{

}

void CImageLoaderJPG::output_message( j_common_ptr cinfo )
{

}
