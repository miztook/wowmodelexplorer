#pragma once

#include "core.h"

class IMemFile;
class IImage;

class CImageLoaderKTX
{
public:
	static bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "ktx"); }

	IImage* loadAsImage( IMemFile* file );
};
