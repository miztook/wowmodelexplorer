#pragma once

#include "base.h"
#include "IWriteFile.h"

class CWriteFile : public IWriteFile
{
public:
	CWriteFile(const c8* filename, bool binary, bool append = false);
	~CWriteFile();

public:
	virtual s32 write(const void* buffer, u32 sizeToWrite);
	virtual s32 writeLine( const c8* buffer, u32 len = MAX_WRITE_NUM );
	virtual bool seek(long finalPos, bool relativeMovement = false);
	virtual s32 getSize() const { return FileSize; }
	virtual s32 getPos() const;
	virtual const c8* getFileName() const { return FileName; }
	virtual bool isOpen() const { return File != NULL;}
	virtual bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary, bool append);

	bool IsBinary;
	c8 FileName[MAX_PATH];
	FILE*	File;
	s32	FileSize;
};