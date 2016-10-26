#include "stdafx.h"
#include "CFileADTLoader.h"
#include "CFileADT.h"

IFileADT* CADTLoader::loadADT( IMemFile* file, bool simple )
{
	CFileADT* adtFile = new CFileADT;
	if (simple)
	{
		if (!adtFile->loadFileSimple(file))
		{
			adtFile->drop();
			return NULL_PTR;
		}
	}
	else
	{
		if (!adtFile->loadFile(file))
		{
			adtFile->drop();
			return NULL_PTR;
		}
	}

	return adtFile;
}

IFileADT* CADTLoader::loadADTTextures(IMemFile* file)
{
	CFileADT* adtFile = new CFileADT;
	if (!adtFile->loadFileTextures(file))
	{
		adtFile->drop();
		return NULL_PTR;
	}
	
	return adtFile;
}
