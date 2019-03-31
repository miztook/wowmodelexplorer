#include "stdafx.h"
#include "editor_FileWDT.h"
#include "CFileWDT.h"

uint32_t FileWDT_getTileCount( IFileWDT* wdt )
{
	return static_cast<CFileWDT*>(wdt)->getTileCount();
}

bool FileWDT_getTile( IFileWDT* wdt, uint32_t index, uint32_t* row, uint32_t* col )
{
	STile* tile = static_cast<CFileWDT*>(wdt)->getTile(index);
	if (!tile)
		return false;
	*row = tile->row;
	*col = tile->col;
	return true;
}

IFileADT* FileWDT_loadADT( IFileWDT* wdt, uint32_t row, uint32_t col, bool simple )
{
	CFileWDT* fileWDT = static_cast<CFileWDT*>(wdt);
	STile* tile = fileWDT->getTile((uint8_t)row, (uint8_t)col);
	if (!tile)
		return NULL;

	if(fileWDT->loadADT(tile, simple))
	{
		return tile->fileAdt;
	}
	return NULL;
}

IFileADT* FileWDT_loadADTTextures(IFileWDT* wdt, uint32_t row, uint32_t col)
{
	CFileWDT* fileWDT = static_cast<CFileWDT*>(wdt);
	STile* tile = fileWDT->getTile((uint8_t)row, (uint8_t)col);
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
	for (uint32_t i=0; i<fileWDT->getTileCount(); ++i)
	{
		STile* tile = fileWDT->getTile(i);
		if (tile->fileAdt == adt && tile->fileAdt)
		{
			fileWDT->unloadADT(tile);
			break;
		}
	}
}


