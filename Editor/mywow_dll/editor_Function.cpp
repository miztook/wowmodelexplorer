#include "stdafx.h"
#include "editor_Function.h"

void Function_getFileDirA(const char* filename, char* outfilename, uint32_t size)
{
	getFileDirA(filename, outfilename, size);
}

void Function_getFileNameA(const char* filename, char* outfilename, uint32_t size)
{
	getFileNameA(filename, outfilename, size);
}

void Function_getFileNameNoExtensionA(const char* filename, char* outfilename, uint32_t size)
{
	getFileNameNoExtensionA(filename, outfilename, size);
}

bool Function_hasFileExtensionA(const char* filename, const char* ext)
{
	return hasFileExtensionA(filename, ext);
}
