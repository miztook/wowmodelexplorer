#include "stdafx.h"

#include "editor_FileADT.h"
#include "CFileADT.h"

char g_m2name[DEFAULT_SIZE];
char g_wmoname[DEFAULT_SIZE];
char g_texname[DEFAULT_SIZE];

uint32_t FileADT_getM2Count( IFileADT* adt )
{
	return adt->NumM2FileNames;
}

uint32_t FileADT_getWMOCount( IFileADT* adt )
{
	return adt->NumWmoFileNames;
}

uint32_t FileADT_getTextureCount(IFileADT* adt)
{
	return static_cast<CFileADT*>(adt)->getNumTextures();
}

const char* FileADT_getM2FileName( IFileADT* adt, uint32_t index, bool shortname )
{
	const char* filename = static_cast<CFileADT*>(adt)->getM2FileName(index);
	if (shortname)
	{
		getFileNameA(filename, g_m2name, DEFAULT_SIZE);
		return g_m2name;
	}
	else
	{
		return filename;
	}
}

const char* FileADT_getWMOFileName( IFileADT* adt, uint32_t index, bool shortname )
{
	const char* filename =  static_cast<CFileADT*>(adt)->getWMOFileName(index);
	if (shortname)
	{
		getFileNameA(filename, g_wmoname, DEFAULT_SIZE);
		return g_wmoname;
	}
	else
	{
		return filename;
	}
}

const char* FileADT_getTextureFileName(IFileADT* adt, uint32_t index, bool shortname)
{
	const char* filename =  static_cast<CFileADT*>(adt)->getTextureName(index);
	if (shortname)
	{
		getFileNameA(filename, g_texname, DEFAULT_SIZE);
		return g_texname;
	}
	else
	{
		return filename;
	}
}
