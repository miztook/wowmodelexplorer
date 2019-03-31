#pragma once

#include "IReadFile.h"
#include <cstdio>

class CReadFile : public IReadFile
{
public:
	CReadFile(const char* filename, bool binary);
	virtual ~CReadFile();

public:
	virtual uint32_t read(void* buffer, uint32_t sizeToRead);
	virtual uint32_t readText(char* buffer, uint32_t len = MAX_READ_NUM);
	virtual uint32_t readLine(char* buffer, uint32_t len = MAX_READ_NUM);
	virtual uint32_t readLineSkipSpace(char* buffer, uint32_t len = MAX_READ_NUM);
	virtual bool seek(int32_t finalPos, bool relativePos = false);
	virtual uint32_t getSize() const { return FileSize; }
	virtual int32_t getPos() const;
	virtual bool isEof() const;
	virtual const char* getFileName() const { return FileName; }
	virtual bool isOpen() const { return File != nullptr; }
	virtual bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary);
	bool isWhiteSpace(const char symbol) const
	{
		return symbol == ' ' || symbol == '\t' || symbol == '\r';
	}

private:
	FILE*		File;
	uint32_t		FileSize;
	char		FileName[QMAX_PATH];
	bool		IsBinary;
};