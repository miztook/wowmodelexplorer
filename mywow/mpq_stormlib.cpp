#include "stdafx.h"
#include "mpq_stormlib.h"
#include "StormLib.h"

MPQArchive::MPQArchive( const c8* filename )
	: ok(false), archivename(filename)
{
	bool result = SFileOpenArchive(filename, 0, MPQ_OPEN_FORCE_MPQ_V1|MPQ_OPEN_READ_ONLY, &mpq_a );

	_ASSERT(result);

	ok = result;
}

MPQArchive::~MPQArchive()
{
	close();
}

void MPQArchive::applyPatch( const c8* patchnames[], u32 num )
{
	c8 dir[MAX_PATH];
	getFileDirA(archivename.c_str(), dir, MAX_PATH);
	for (u32 i=0; i<num; ++i)
	{
		string256 path = dir;
		path.append("\\");
		path.append(patchnames[i]);
		SFileOpenPatchArchive(mpq_a, path.c_str(), "", 0);
	}
}

void MPQArchive::close()
{
	if (ok)
	{
		SFileCloseArchive(mpq_a);
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

bool MPQFile::seek( s32 offset, bool relative )
{
	if (relative)
		pointer += offset;
	else
		pointer = offset;
	eof = (pointer >= size);
	return !eof;
}

void MPQFile::close()
{
	if(temp)
		Hunk_FreeTempMemory(buffer);
	else
		delete[] buffer;
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
