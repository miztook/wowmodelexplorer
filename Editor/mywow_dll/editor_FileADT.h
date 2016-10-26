#pragma once

#include "editor_base.h"

MW_API  u32 FileADT_getM2Count(IFileADT* adt);
MW_API  u32 FileADT_getWMOCount(IFileADT* adt);
MW_API  u32 FileADT_getTextureCount(IFileADT* adt);

MW_API  const c8* FileADT_getM2FileName(IFileADT* adt, u32 index, bool shortname);
MW_API  const c8* FileADT_getWMOFileName(IFileADT* adt, u32 index, bool shortname);
MW_API  const c8* FileADT_getTextureFileName(IFileADT* adt, u32 index, bool shortname);