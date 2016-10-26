#include "stdafx.h"
#include "CFileWMOLoader.h"
#include "CFileWMO.h"

IFileWMO* CWMOLoader::loadWMO( IMemFile* file )
{
	CFileWMO* wmoFile = new CFileWMO;
	if (!wmoFile->loadFile(file))
	{
		wmoFile->drop();
		return NULL_PTR;
	}
	return wmoFile;
}