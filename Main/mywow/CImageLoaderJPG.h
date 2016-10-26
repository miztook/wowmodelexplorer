#pragma once

#include "core.h"
#include "jpeglib.h"

class IMemFile;
class IImage;

class CImageLoaderJPG
{
public:
	static bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "jpg") || hasFileExtensionA(filename, "jpeg"); }

	IImage* loadAsImage( IMemFile* file );

private:
	struct irr_jpeg_error_mgr
	{
		// public jpeg error fields
		struct jpeg_error_mgr pub;

		// for longjmp, to return to caller on a fatal error
		jmp_buf setjmp_buffer;
	};

	static void error_exit (j_common_ptr cinfo);

	static void output_message(j_common_ptr cinfo);

	static void init_source (j_decompress_ptr cinfo);

	static boolean fill_input_buffer (j_decompress_ptr cinfo);

	static void skip_input_data (j_decompress_ptr cinfo, long num_bytes);

	static void term_source (j_decompress_ptr cinfo);
};
