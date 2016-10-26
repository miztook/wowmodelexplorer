#pragma once

#include "core.h"

class IFileM2;
class IMemFile;

class CM2Loader
{
public:
	static bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "m2"); }

	IFileM2* loadM2( IMemFile* file );
};