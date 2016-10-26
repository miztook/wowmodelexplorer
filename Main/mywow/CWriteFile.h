#pragma once

#include "IWriteFile.h"

#include <cstdio>

class CWriteFile : public IWriteFile
{
public:
	CWriteFile(const c8* filename, bool binary, bool append = false);
	~CWriteFile();

public:
	virtual u32 write(const void* buffer, u32 sizeToWrite);
	virtual u32 writeText( const c8* buffer, u32 len = MAX_WRITE_NUM );
	virtual u32 writeLine(const c8* text);
	virtual bool flush();
	virtual bool seek(s32 finalPos, bool relativeMovement = false);
	virtual u32 getSize() const { return FileSize; }
	virtual s32 getPos() const;
	virtual const c8* getFileName() const { return FileName; }
	virtual bool isOpen() const { return File != NULL_PTR;}
	virtual bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary, bool append);

private:
	c8 FileName[QMAX_PATH];
	FILE*	File;
	u32	FileSize;
	bool IsBinary;
};