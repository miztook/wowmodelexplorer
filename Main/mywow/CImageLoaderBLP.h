#pragma once

#include "core.h"

class IMemFile;
class IImage;

class CImageLoaderBLP
{
public:
	static bool isALoadableFileExtension( const char* filename ) { return hasFileExtensionA(filename, "blp"); }

	IImage* loadAsImage( IMemFile* file, bool changeRB );

};

