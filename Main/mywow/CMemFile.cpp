#include "stdafx.h"
#include "CMemFile.h"
#include "temp_memory.h"

u32 CMemFile::read( void* dest, u32 bytes )
{
	if (eof) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > size) {
		bytes = size - pointer;
		eof = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = (u32)rpos;

	return bytes;
}

bool CMemFile::seek( s32 offset, bool relative )
{
	if (relative)
		pointer += offset;
	else
		pointer = offset;
	eof = (pointer >= size);
	return !eof;
}

void CMemFile::close()
{
	if(temp)
		Z_FreeTempMemory(buffer);
	else
		delete[] buffer;
	buffer = NULL_PTR;

	eof = true;
}

bool CMemFile::save( const c8* filename )
{
	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);

	if (!Q_MakeDirForFileName(realfilename))
		return false;
	
	FILE* file = Q_fopen(realfilename, "wb");
	if (file != NULL_PTR)
	{
		fwrite(buffer, 1, size, file);
		fclose(file);
		return true;
	}
	else
	{
		ASSERT(false);
		return false;
	}
}