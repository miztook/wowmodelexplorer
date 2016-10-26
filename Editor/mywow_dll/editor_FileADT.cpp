#include "stdafx.h"

#include "editor_FileADT.h"
#include "CFileADT.h"

c8 g_m2name[DEFAULT_SIZE];
c8 g_wmoname[DEFAULT_SIZE];
c8 g_texname[DEFAULT_SIZE];

u32 FileADT_getM2Count( IFileADT* adt )
{
	return adt->NumM2FileNames;
}

u32 FileADT_getWMOCount( IFileADT* adt )
{
	return adt->NumWmoFileNames;
}

u32 FileADT_getTextureCount(IFileADT* adt)
{
	return static_cast<CFileADT*>(adt)->getNumTextures();
}

const c8* FileADT_getM2FileName( IFileADT* adt, u32 index, bool shortname )
{
	const c8* filename = static_cast<CFileADT*>(adt)->getM2FileName(index);
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

const c8* FileADT_getWMOFileName( IFileADT* adt, u32 index, bool shortname )
{
	const c8* filename =  static_cast<CFileADT*>(adt)->getWMOFileName(index);
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

const c8* FileADT_getTextureFileName(IFileADT* adt, u32 index, bool shortname)
{
	const c8* filename =  static_cast<CFileADT*>(adt)->getTextureName(index);
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
