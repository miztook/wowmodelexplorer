#pragma once

#include "editor_base.h"

class IFileWDT;

MW_API  IFileWDT* ResourceLoader_loadWDT(const char* mapname, int32_t id, bool simple);
MW_API void ResourceLoader_unloadWDT(IFileWDT* wdt);
