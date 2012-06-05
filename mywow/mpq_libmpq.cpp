#include "stdafx.h"
#include "mpq_libmpq.h"

MPQArchive::MPQArchive( const c8* filename )
	: ok(false)
{
	int result = libmpq_archive_open(&mpq_ar, (unsigned char*)filename);

	_ASSERT(result == 0);

	ok = (result == 0);

	isPatch = strstr(filename, "wow-update-") != NULL;
}

MPQArchive::~MPQArchive()
{
	close();
}

void MPQArchive::close()
{
	if (ok)
	{
		libmpq_archive_close(&mpq_ar);
		ok = false;
	}
}

u32 MPQFile::read( void* dest, u32 bytes )
{
	if (eof) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > size) {
		bytes = size - pointer;
		eof = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = rpos;

	return bytes;
}

void MPQFile::seek( s32 offset )
{
	pointer = offset;
	eof = (pointer >= size);
}

void MPQFile::seekRelative( s32 offset )
{
	pointer += offset;
	eof = (pointer >= size);
}

void MPQFile::close()
{
	//delete[] buffer;
	Hunk_FreeTempMemory(buffer);
	buffer = NULL;

	eof = true;
}

void MPQFile::save( const c8* filename )
{
	c16 fname[MAX_PATH];
	utf8to16(filename, fname, MAX_PATH);
	
	FILE* file;
	int err = _wfopen_s(&file, fname, L"wb");
	if (err == 0)
	{
		fwrite(buffer, 1, size, file);
		fclose(file);
	}
	else
	{
		_ASSERT(false);
	}
}
