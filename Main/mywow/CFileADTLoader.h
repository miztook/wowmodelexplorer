#pragma once

#include "core.h"

class IFileADT;
class IMemFile;

class CADTLoader
{
public:
	static bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "adt"); }

	IFileADT* loadADT( IMemFile* file, bool simple );
	IFileADT* loadADTTextures( IMemFile* file);
};
