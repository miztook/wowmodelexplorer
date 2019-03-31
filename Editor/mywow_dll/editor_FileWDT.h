#pragma once

#include "editor_base.h"

class IFileWDT;
class IFileADT;

MW_API  uint32_t FileWDT_getTileCount(IFileWDT* wdt);
MW_API  bool FileWDT_getTile(IFileWDT* wdt, uint32_t index, uint32_t* row, uint32_t* col);

MW_API IFileADT* FileWDT_loadADT(IFileWDT* wdt, uint32_t row, uint32_t col, bool simple);
MW_API IFileADT* FileWDT_loadADTTextures(IFileWDT* wdt, uint32_t row, uint32_t col);
MW_API void FileWDT_unloadADT(IFileWDT* wdt, IFileADT* adt);
