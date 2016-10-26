#include "stdafx.h"
#include "CFileWDTLoader.h"
#include "CFileWDT.h"

IFileWDT* CWDTLoader::loadWDT( IMemFile* file, s32 mapid, bool simple )
{
	CFileWDT* wdtFile = new CFileWDT;
	if (simple)
	{
		if (!wdtFile->loadFileSimple(file, mapid))
		{
			wdtFile->drop();
			return NULL_PTR;
		}
	}
	else
	{
		if (!wdtFile->loadFile(file, mapid))
		{
			wdtFile->drop();
			return NULL_PTR;
		}
	}

	return wdtFile;
}