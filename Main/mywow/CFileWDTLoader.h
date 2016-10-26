#pragma once

#include "core.h"

class IFileWDT;
class IMemFile;

class CWDTLoader
{
public:
	static bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "wdt"); }

	IFileWDT* loadWDT( IMemFile* file, s32 mapid, bool simple );
};
