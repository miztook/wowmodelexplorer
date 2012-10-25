#pragma once

#include "base.h"

#define	MAX_READ_NUM		1024

class IReadFile
{
public:
	virtual ~IReadFile() { }

public:
	virtual u32 read(void* buffer, u32 sizeToRead) = 0;
	virtual u32 readLine(c8* buffer, u32 len = MAX_READ_NUM ) = 0;
	virtual bool seek(s32 finalPos, bool relativePos = false) = 0;
	virtual u32 getSize() const = 0;
	virtual s32 getPos() const = 0;
	virtual bool isEof() const = 0;
	virtual const c8* getFileName() const= 0;
	virtual bool isOpen() const = 0;
	virtual bool isBinary() const = 0;
};