#pragma once

#include "editor_base.h"

MW_API  uint32_t FileADT_getM2Count(IFileADT* adt);
MW_API  uint32_t FileADT_getWMOCount(IFileADT* adt);
MW_API  uint32_t FileADT_getTextureCount(IFileADT* adt);

MW_API  const char* FileADT_getM2FileName(IFileADT* adt, uint32_t index, bool shortname);
MW_API  const char* FileADT_getWMOFileName(IFileADT* adt, uint32_t index, bool shortname);
MW_API  const char* FileADT_getTextureFileName(IFileADT* adt, uint32_t index, bool shortname);
