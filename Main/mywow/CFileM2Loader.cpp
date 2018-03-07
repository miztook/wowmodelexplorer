#include "stdafx.h"
#include "CFileM2Loader.h"
#include "CFileM2.h"

IFileM2* CM2Loader::loadM2( IMemFile* file )
{
	CFileM2* m2File = new CFileM2;

	if (!m2File->loadFile(file))
	{
		m2File->drop();	
		return nullptr;
	}

	return m2File;
}