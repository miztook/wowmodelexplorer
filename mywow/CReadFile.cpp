#include "stdafx.h"
#include "CReadFile.h"

CReadFile::CReadFile( const char* filename, bool binary )
{
	strcpy_s(FileName, MAX_PATH, filename);
	openFile(binary);
}

CReadFile::~CReadFile()
{
	if(File)
		fclose(File);
}

u32 CReadFile::read( void* buffer, u32 sizeToRead )
{
	if( !isOpen() )
		return 0;
	return fread(buffer, 1, sizeToRead, File );
}

u32 CReadFile::readLine( c8* buffer, u32 len /*= MAX_READ_NUM */ )
{
	if ( !isOpen() )
		return 0;

	c8* c = fgets(buffer, len, File);
	return c ? strlen(c) : 0;
}

bool CReadFile::seek( s32 finalPos, bool relativePos/* = false*/ )
{
	if( !isOpen() )
		return false;

	return fseek( File, finalPos, relativePos ? SEEK_CUR : SEEK_SET ) == 0;
}

s32 CReadFile::getPos() const
{
	return ftell(File);
}

void CReadFile::openFile( bool binary )
{
	if (fopen_s(&File, FileName, binary ? "rb": "rt") == 0)
	{
		fseek( File, 0, SEEK_END );
		long size = ftell(File);
		FileSize = size > 0 ? (u32)size : 0;
		fseek( File, 0, SEEK_SET );
	}
	else
	{
		File = NULL;
		FileSize = 0;
	}
}

bool CReadFile::isEof() const
{
	return getPos() == FileSize;
}