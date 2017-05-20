#include "stdafx.h"
#include "CReadFile.h"
#include "core.h"

CReadFile::CReadFile(const char* filename, bool binary)
: IsBinary(binary)
{
	Q_strcpy(FileName, QMAX_PATH, filename);
	normalizeFileName(FileName);
	openFile(IsBinary);
}

CReadFile::~CReadFile()
{
	if (File)
		fclose(File);
}

u32 CReadFile::read(void* buffer, u32 sizeToRead)
{
	if (!isOpen() || buffer == NULL_PTR)
		return 0;

	return (u32)fread(buffer, 1, sizeToRead, File);
}

u32 CReadFile::readText(c8* buffer, u32 len /*= MAX_READ_NUM */)
{
	if (!isOpen() || buffer == NULL_PTR)
		return 0;

	ASSERT(!IsBinary);

	c8* c = fgets(buffer, len, File);
	return c ? (u32)strlen(c) : 0;
}

u32 CReadFile::readLine(c8* buffer, u32 len /*= MAX_READ_NUM*/)
{
	if (!isOpen() || buffer == NULL_PTR)
		return 0;

	ASSERT(!IsBinary);

	int c = fgetc(File);
	u32 count = 0;
	while (c != '\n' && c != EOF)
	{
		if (1 + count >= len)
			break;
		buffer[count] = c;
		++count;

		c = fgetc(File);
	}

	return count;
}

u32 CReadFile::readLineSkipSpace(c8* buffer, u32 len /*= MAX_READ_NUM*/)
{
	if (!isOpen() || buffer == NULL_PTR)
		return 0;

	memset(buffer, 0, len);

	ASSERT(!IsBinary);

	int c = fgetc(File);
	u32 count = 0;

	while (c != '\n' && c != EOF)
	{
		if (1 + count >= len)
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

bool CReadFile::seek(s32 finalPos, bool relativePos/* = false*/)
{
	if (!isOpen())
		return false;

	return fseek(File, finalPos, relativePos ? SEEK_CUR : SEEK_SET) == 0;
}

s32 CReadFile::getPos() const
{
	return (s32)ftell(File);
}

void CReadFile::openFile(bool binary)
{
	File = Q_fopen(FileName, binary ? "rb" : "rt");
	if (File)
	{
		fseek(File, 0, SEEK_END);
		long size = ftell(File);
		FileSize = size > 0 ? (u32)size : 0;
		fseek(File, 0, SEEK_SET);
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