#pragma once

#include "editor_base.h"

MW_API void Function_getFileDirA(const char* filename, char* outfilename, uint32_t size);
MW_API void Function_getFileNameA(const char* filename, char* outfilename, uint32_t size);
MW_API void Function_getFileNameNoExtensionA(const char* filename, char* outfilename, uint32_t size);
MW_API bool Function_hasFileExtensionA( const char* filename, const char* ext );
