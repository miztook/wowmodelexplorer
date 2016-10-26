#include "stdafx.h"
#include "CWriteFile.h"

CWriteFile::CWriteFile( const c8* filename, bool binary, bool append /*= false*/ )
	: IsBinary(binary)
{
	strcpy_s(FileName, MAX_PATH, filename);
	openFile(IsBinary, append);
}

CWriteFile::~CWriteFile()
{
	if(File)
		fclose(File);
}

u32 CWriteFile::write( const void* buffer, u32 sizeToWrite )
{
	if (!isOpen() || buffer==NULL)
		return 0;

	return fwrite(buffer, 1, sizeToWrite, File);
}

u32 CWriteFile::writeText( const c8* buffer, u32 len /*= MAX_WRITE_NUM */ )
{
	if (!isOpen() || buffer==NULL )
		return 0;

	_ASSERT(!IsBinary);

	if(strlen(buffer) > len)
	{
		_ASSERT(false);
		return 0;
	}

	s32 w1 = fputs(buffer, File);
	if (w1 == EOF)
		return 0;

	return (u32)w1;
}

bool CWriteFile::flush()
{
	if (isOpen())
	{
		return 0 == fflush(File);
	}
	return false;
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

	File = _fsopen(FileName, mode, _SH_DENYWR);
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

