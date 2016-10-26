#include "stdafx.h"
#include "CWriteFile.h"
#include "core.h"

CWriteFile::CWriteFile( const c8* filename, bool binary, bool append /*= false*/ )
	: IsBinary(binary)
{
	Q_strcpy(FileName, QMAX_PATH, filename);
	normalizeFileName(FileName);
	openFile(IsBinary, append);
}

CWriteFile::~CWriteFile()
{
	if(File)
		fclose(File);
}

u32 CWriteFile::write( const void* buffer, u32 sizeToWrite )
{
	if (!isOpen() || buffer==NULL_PTR)
		return 0;

	return (u32)fwrite(buffer, 1, sizeToWrite, File);
}

u32 CWriteFile::writeText( const c8* buffer, u32 len /*= MAX_WRITE_NUM */ )
{
	if (!isOpen() || buffer==NULL_PTR )
		return 0;

	ASSERT(!IsBinary);

	if(strlen(buffer) > len)
	{
		ASSERT(false);
		return 0;
	}

	s32 w1 = fputs(buffer, File);
	if (w1 == EOF)
		return 0;

	return (u32)w1;
}

u32 CWriteFile::writeLine(const c8* text)
{
	if (!isOpen() || text==NULL_PTR )
		return 0;

	ASSERT(!IsBinary);

	s32 w = fprintf(File, "%s\n", text);
	if (w == EOF)
		return 0;

	return (u32)w;
}

bool CWriteFile::flush()
{
	if (isOpen())
	{
		return 0 == fflush(File);
	}
	return false;
}

bool CWriteFile::seek( s32 finalPos, bool relativeMovement /*= false*/ )
{
	if (!isOpen())
		return false;

	return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

s32 CWriteFile::getPos() const
{
	return (s32)ftell(File);
}

void CWriteFile::openFile( bool binary, bool append )
{
	const char* mode;
	if (append)
	{
		mode = binary ? "ab" : "at"; 
	}
	else
	{
		mode = binary ? "wb" : "wt";
	}

	File = Q_fopen(FileName, mode);

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

