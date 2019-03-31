#include "stdafx.h"
#include "CWriteFile.h"
#include "core.h"

CWriteFile::CWriteFile(const char* filename, bool binary, bool append /*= false*/)
: IsBinary(binary)
{
	Q_strcpy(FileName, QMAX_PATH, filename);
	normalizeFileName(FileName);
	openFile(IsBinary, append);
}

CWriteFile::~CWriteFile()
{
	if (File)
		fclose(File);
}

uint32_t CWriteFile::write(const void* buffer, uint32_t sizeToWrite)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	return (uint32_t)fwrite(buffer, 1, sizeToWrite, File);
}

uint32_t CWriteFile::writeText(const char* buffer, uint32_t len /*= MAX_WRITE_NUM */)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	ASSERT(!IsBinary);

	if (strlen(buffer) > len)
	{
		ASSERT(false);
		return 0;
	}

	int32_t w1 = fputs(buffer, File);
	if (w1 == EOF)
		return 0;

	return (uint32_t)w1;
}

uint32_t CWriteFile::writeLine(const char* text)
{
	if (!isOpen() || text == nullptr)
		return 0;

	ASSERT(!IsBinary);

	int32_t w = fprintf(File, "%s\n", text);
	if (w == EOF)
		return 0;

	return (uint32_t)w;
}

bool CWriteFile::flush()
{
	if (isOpen())
	{
		return 0 == fflush(File);
	}
	return false;
}

bool CWriteFile::seek(int32_t finalPos, bool relativeMovement /*= false*/)
{
	if (!isOpen())
		return false;

	return fseek(File, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

int32_t CWriteFile::getPos() const
{
	return (int32_t)ftell(File);
}

void CWriteFile::openFile(bool binary, bool append)
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
		fseek(File, 0, SEEK_END);
		long size = ftell(File);
		FileSize = size > 0 ? (uint32_t)size : 0;
		fseek(File, 0, SEEK_SET);
	}
	else
	{
		FileSize = 0;
	}
}