#pragma once

#include "editor_base.h"

MW_API bool WowEnvironment_Exists(const c8* filename);
MW_API u32 WowEnvironment_GetCascFileCount();
MW_API const c8* WowEnvironment_GetCascFile(int index);

MW_API void WowEnvironment_ClearOwnCascFiles();
MW_API void WowEnvironment_AddOwnCascFile(const c8* filename);
MW_API void WowEnvironment_FinishOwnCascFiles();

MW_API int WowEnvironment_GetFileCount_InDir(const c8* baseDir, const c8* ext, bool useOwn);
MW_API void WowEnvironment_GetFile_InDir(bool fullname, int index, c8* outfilename, u32 size);
MW_API int WowEnvironment_GetDirectoryCount_InDir(const c8* baseDir, bool useOwn);
MW_API void WowEnvironment_GetDirectory_InDir(bool fullname, int index, c8* outdirname, u32 size);

MW_API const c8* WowEnvironment_GetLocale();

void WowEnvironment_Clear();