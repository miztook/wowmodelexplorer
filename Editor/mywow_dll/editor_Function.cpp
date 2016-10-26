#include "stdafx.h"
#include "editor_Function.h"

void Function_getFileDirA(const c8* filename, c8* outfilename, u32 size)
{
	getFileDirA(filename, outfilename, size);
}

void Function_getFileNameA(const c8* filename, c8* outfilename, u32 size)
{
	getFileNameA(filename, outfilename, size);
}

void Function_getFileNameNoExtensionA(const c8* filename, c8* outfilename, u32 size)
{
	getFileNameNoExtensionA(filename, outfilename, size);
}

bool Function_hasFileExtensionA(const c8* filename, const c8* ext)
{
	return hasFileExtensionA(filename, ext);
}
