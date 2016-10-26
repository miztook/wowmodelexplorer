#include "stdafx.h"
#include "editor_FileWDT.h"
#include "CFileWDT.h"

u32 FileWDT_getTileCount( IFileWDT* wdt )
{
	return static_cast<CFileWDT*>(wdt)->getTileCount();
}

bool FileWDT_getTile( IFileWDT* wdt, u32 index, u32* row, u32* col )
{
	STile* tile = static_cast<CFileWDT*>(wdt)->getTile(index);
	if (!tile)
		return false;
	*row = tile->row;
	*col = tile->col;
	return true;
}

IFileADT* FileWDT_loadADT( IFileWDT* wdt, u32 row, u32 col, bool simple )
{
	CFileWDT* fileWDT = static_cast<CFileWDT*>(wdt);
	STile* tile = fileWDT->getTile((u8)row, (u8)col);
	if (!tile)
		return NULL;

	if(fileWDT->loadADT(tile, simple))
	{
		return tile->fileAdt;
	}
	return NULL;
}

IFileADT* FileWDT_loadADTTextures(IFileWDT* wdt, u32 row, u32 col)
{
	CFileWDT* fileWDT = static_cast<CFileWDT*>(wdt);
	STile* tile = fileWDT->getTile((u8)row, (u8)col);
	if (!tile)
		return NULL;

	if(fileWDT->loadADTTextures(tile))
	{
		return tile->fileAdt;
	}
	return NULL;
}

void FileWDT_unloadADT( IFileWDT* wdt, IFileADT* adt )
{
	CFileWDT* fileWDT = static_cast<CFileWDT*>(wdt);
	for (u32 i=0; i<fileWDT->getTileCount(); ++i)
	{
		STile* tile = fileWDT->getTile(i);
		if (tile->fileAdt == adt && tile->fileAdt)
		{
			fileWDT->unloadADT(tile);
			break;
		}
	}
}


