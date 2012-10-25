#include "stdafx.h"
#include "CWriteFile.h"

CWriteFile::CWriteFile( const c8* filename, bool binary, bool append /*= false*/ )
{
	strcpy_s(FileName, MAX_PATH, filename);
	openFile(binary, append);
}

CWriteFile::~CWriteFile()
{
	if(File)
		fclose(File);
}

u32 CWriteFile::write( const void* buffer, u32 sizeToWrite )
{
	if (!isOpen())
		return 0;

	return fwrite(buffer, 1, sizeToWrite, File);
}

u32 CWriteFile::writeLine( const c8* buffer, u32 len /*= MAX_WRITE_NUM */ )
{
	if (!isOpen() || buffer==NULL )
		return 0;

	if(strlen(buffer) > len)
	{
		_ASSERT(false);
		return 0;
	}

	s32 w1 = fputs(buffer, File);
	if (w1 == EOF)
		return 0;

	s32 w2 = fputc('\n', File);
	if (w2 == EOF)
		return 0;

	return (u32)(w1+w2);
}

bool CWriteFile::seek( long finalPos, bool relativeMovement /*= false*/ )
{
	if (!isOpen())
		return false;

	return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

s32 CWriteFile::getPos() const
{
	return ftell(File);
}

void CWriteFile::openFile( bool binary, bool append )
{
	const TCHAR* mode;
	if (append)
	{
		mode = binary ? "ab" : "at"; 
	}
	else
	{
		mode = binary ? "wb" : "wt";
	}

	if (fopen_s(&File, FileName, mode) == 0)
	{
		fseek( File, 0, SEEK_END );
		long size = ftell(File);
		FileSize = size > 0 ? (u32)size : 0;
		fseek( File, 0, SEEK_SET );
	}
	else
	{
		File = 0;
	}
}