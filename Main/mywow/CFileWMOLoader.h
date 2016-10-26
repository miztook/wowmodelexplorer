#pragma once

#include "core.h"

class IFileWMO;
class IMemFile;

class CWMOLoader
{
public:
	static bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "wmo"); }

	IFileWMO* loadWMO( IMemFile* file );
};
