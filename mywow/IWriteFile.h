#pragma once

#include "base.h"

#define MAX_WRITE_NUM	1024

class IWriteFile
{
public:
	virtual ~IWriteFile() { }

public:
	virtual s32 write(const void* buffer, u32 sizeToWrite) = 0;
	virtual s32 writeLine( const c8* buffer, u32 len = MAX_WRITE_NUM ) = 0;
	virtual bool seek(long finalPos, bool relativeMovement = false) = 0;
	virtual s32 getSize() const = 0;
	virtual s32 getPos() const = 0;
	virtual const c8* getFileName() const = 0;
	virtual bool isOpen() const = 0;
	virtual bool isBinary() const = 0;
};