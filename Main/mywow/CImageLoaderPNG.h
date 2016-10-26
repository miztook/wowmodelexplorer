#pragma once

#include "core.h"

class IMemFile;
class IImage;

class CImageLoaderPNG
{
public:
	static bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "png"); }

	IImage* loadAsImage( IMemFile* file);
};