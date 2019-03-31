#include "stdafx.h"
#include "CMemFile.h"
#include "temp_memory.h"

uint32_t CMemFile::read( void* dest, uint32_t bytes )
{
	if (eof) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > size) {
		bytes = size - pointer;
		eof = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = (uint32_t)rpos;

	return bytes;
}

bool CMemFile::seek( int32_t offset, bool relative )
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
	buffer = nullptr;

	eof = true;
}

bool CMemFile::save( const char* filename )
{
	char realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);

	if (!Q_MakeDirForFileName(realfilename))
		return false;
	
	FILE* file = Q_fopen(realfilename, "wb");
	if (file != nullptr)
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