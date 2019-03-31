#pragma once

#include "IWriteFile.h"

#include <cstdio>

class CWriteFile : public IWriteFile
{
public:
	CWriteFile(const char* filename, bool binary, bool append = false);
	~CWriteFile();

public:
	virtual uint32_t write(const void* buffer, uint32_t sizeToWrite);
	virtual uint32_t writeText( const char* buffer, uint32_t len = MAX_WRITE_NUM );
	virtual uint32_t writeLine(const char* text);
	virtual bool flush();
	virtual bool seek(int32_t finalPos, bool relativeMovement = false);
	virtual uint32_t getSize() const { return FileSize; }
	virtual int32_t getPos() const;
	virtual const char* getFileName() const { return FileName; }
	virtual bool isOpen() const { return File != nullptr;}
	virtual bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary, bool append);

private:
	char FileName[QMAX_PATH];
	FILE*	File;
	uint32_t	FileSize;
	bool IsBinary;
};