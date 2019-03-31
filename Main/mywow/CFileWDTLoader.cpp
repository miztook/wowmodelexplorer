#include "stdafx.h"
#include "CFileWDTLoader.h"
#include "CFileWDT.h"

IFileWDT* CWDTLoader::loadWDT( IMemFile* file, int32_t mapid, bool simple )
{
	CFileWDT* wdtFile = new CFileWDT;
	if (simple)
	{
		if (!wdtFile->loadFileSimple(file, mapid))
		{
			wdtFile->drop();
			return nullptr;
		}
	}
	else
	{
		if (!wdtFile->loadFile(file, mapid))
		{
			wdtFile->drop();
			return nullptr;
		}
	}

	return wdtFile;
}