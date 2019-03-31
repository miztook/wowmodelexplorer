#pragma once

#include "core.h"

class IMemFile;
class IImage;

class CImageLoaderPVR
{
public:
	static bool isALoadableFileExtension( const char* filename ) { return hasFileExtensionA(filename, "pvr"); }

	IImage* loadAsImage( IMemFile* file );

};

