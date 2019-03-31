#pragma once

#include "editor_base.h"

MW_API bool WowEnvironment_Exists(const char* filename);
MW_API uint32_t WowEnvironment_GetCascFileCount();
MW_API const char* WowEnvironment_GetCascFile(int index);

MW_API void WowEnvironment_ClearOwnCascFiles();
MW_API void WowEnvironment_AddOwnCascFile(const char* filename);
MW_API void WowEnvironment_FinishOwnCascFiles();

MW_API int WowEnvironment_GetFileCount_InDir(const char* baseDir, const char* ext, bool useOwn);
MW_API void WowEnvironment_GetFile_InDir(bool fullname, int index, char* outfilename, uint32_t size);
MW_API int WowEnvironment_GetDirectoryCount_InDir(const char* baseDir, bool useOwn);
MW_API void WowEnvironment_GetDirectory_InDir(bool fullname, int index, char* outdirname, uint32_t size);

MW_API const char* WowEnvironment_GetLocale();

void WowEnvironment_Clear();