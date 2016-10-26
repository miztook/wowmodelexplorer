#pragma once

#define AFILE_LINEMAXLEN			2048

bool AUX_CopyFile(const char* dst, const char* src);

bool AUX_CreateDirectory(const char* szDir);

bool AUX_ExportBlpAsTga(const char* blpfilename, const char* tgafilename, bool bAlpha);

bool AUX_ExportBlpAsTgaDir(const char* blpfilename, const char* dirname, bool bAlpha);