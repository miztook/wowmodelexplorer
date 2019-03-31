#pragma once

#include "core.h"

class IFileWDT;
class IMemFile;

class CWDTLoader
{
public:
	static bool isALoadableFileExtension( const char* filename ) { return hasFileExtensionA(filename, "wdt"); }

	IFileWDT* loadWDT( IMemFile* file, int32_t mapid, bool simple );
};
