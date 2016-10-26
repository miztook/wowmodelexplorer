#pragma once

#include "editor_base.h"

class IFileWDT;
class IFileADT;

MW_API  u32 FileWDT_getTileCount(IFileWDT* wdt);
MW_API  bool FileWDT_getTile(IFileWDT* wdt, u32 index, u32* row, u32* col);

MW_API IFileADT* FileWDT_loadADT(IFileWDT* wdt, u32 row, u32 col, bool simple);
MW_API IFileADT* FileWDT_loadADTTextures(IFileWDT* wdt, u32 row, u32 col);
MW_API void FileWDT_unloadADT(IFileWDT* wdt, IFileADT* adt);
