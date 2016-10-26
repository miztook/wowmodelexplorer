#pragma once

#include "editor_base.h"

MW_API void Function_getFileDirA(const c8* filename, c8* outfilename, u32 size);
MW_API void Function_getFileNameA(const c8* filename, c8* outfilename, u32 size);
MW_API void Function_getFileNameNoExtensionA(const c8* filename, c8* outfilename, u32 size);
MW_API bool Function_hasFileExtensionA( const c8* filename, const c8* ext );