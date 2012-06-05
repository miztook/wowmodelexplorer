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

s32 CReadFile::read( void* buffer, u32 sizeToRead )
{
	if( !isOpen() )
		return 0;
	return (s32)fread(buffer, 1, sizeToRead, File );
}

s32 CReadFile::readLine( c8* buffer, u32 len /*= MAX_READ_NUM */ )
{
	if ( !isOpen() )
		return 0;

	c8* c = fgets(buffer, len, File);
	return c ? strlen(c) : 0;
}

bool CReadFile::seek( long finalPos, bool relativeMovement /*= false*/ )
{
	if( !isOpen() )
		return false;
	return fseek( File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET ) == 0;
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
		FileSize = ftell( File );
		fseek( File, 0, SEEK_SET );
	}
	else
	{
		File = NULL;
		FileSize = 0;
	}
}