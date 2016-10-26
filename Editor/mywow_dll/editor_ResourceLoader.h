#pragma once

#include "editor_base.h"

class IFileWDT;

MW_API  IFileWDT* ResourceLoader_loadWDT(const c8* mapname, s32 id, bool simple);
MW_API void ResourceLoader_unloadWDT(IFileWDT* wdt);
