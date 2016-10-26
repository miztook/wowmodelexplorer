#include "stdafx.h"
#include "CReadFile.h"

CReadFile::CReadFile( const char* filename, bool binary )
	: IsBinary(binary)
{
	strcpy_s(FileName, MAX_PATH, filename);
	openFile(IsBinary);
}

CReadFile::~CReadFile()
{
	if(File)
		fclose(File);
}

u32 CReadFile::read( void* buffer, u32 sizeToRead )
{
	if( !isOpen() || buffer==NULL )
		return 0;

	return fread(buffer, 1, sizeToRead, File );
}

u32 CReadFile::readText( c8* buffer, u32 len /*= MAX_READ_NUM */ )
{
	if ( !isOpen() || buffer==NULL)
		return 0;

	_ASSERT(!IsBinary);

	c8* c = fgets(buffer, len, File);
	return c ? strlen(c) : 0;
}

u32 CReadFile::readLine( c8* buffer, u32 len /*= MAX_READ_NUM*/ )
{
	if ( !isOpen() || buffer==NULL)
		return 0;

	_ASSERT(!IsBinary);

	int c = fgetc(File);
	u32 count = 0;
	while(c != '\n' && c != EOF)
	{
		if (count > len)
			break;
		buffer[count] = c;
		++count;

		c = fgetc(File);
	}

	return count;
}

u32 CReadFile::readLineSkipSpace( c8* buffer, u32 len /*= MAX_READ_NUM*/ )
{
	if ( !isOpen() || buffer==NULL)
		return 0;

	memset(buffer, 0, len);

	_ASSERT(!IsBinary);

	int c = fgetc(File);
	u32 count = 0;

	while(c != '\n' && c != EOF)
	{
		if (count > len)
			break;

		if (!isWhiteSpace((c8)c))
		{
			buffer[count] = c;
			++count;
		}
		c = fgetc(File);
	}

	return count;
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
	File = _fsopen(FileName, binary ? "rb": "rt", _SH_DENYWR);
	if (File)
	{
		fseek( File, 0, SEEK_END );
		long size = ftell(File);
		FileSize = size > 0 ? (u32)size : 0;
		fseek( File, 0, SEEK_SET );
	}
	else
	{
		FileSize = 0;
	}
}

bool CReadFile::isEof() const
{
	return getPos() == (s32)FileSize;
}

