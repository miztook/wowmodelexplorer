#pragma once

#include "IReadFile.h"
#include <cstdio>

class CReadFile : public IReadFile
{
public:
	CReadFile(const char* filename, bool binary);
	virtual ~CReadFile();

public:
	virtual u32 read(void* buffer, u32 sizeToRead);
	virtual u32 readText(c8* buffer, u32 len = MAX_READ_NUM );
	virtual u32 readLine(c8* buffer, u32 len = MAX_READ_NUM);
	virtual u32 readLineSkipSpace(c8* buffer, u32 len = MAX_READ_NUM);
	virtual bool seek(s32 finalPos, bool relativePos = false);
	virtual u32 getSize() const { return FileSize; }
	virtual s32 getPos() const;
	virtual bool isEof() const;
	virtual const c8* getFileName() const { return FileName; }
	virtual bool isOpen() const { return File != NULL_PTR; }
	virtual bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary);
	bool isWhiteSpace( const c8 symbol ) const
	{
		return symbol == ' ' || symbol == '\t' || symbol == '\r';
	}

private:
	FILE*		File;
	u32		FileSize;
	c8		FileName[QMAX_PATH];
	bool		IsBinary;
};