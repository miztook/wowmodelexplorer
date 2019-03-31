#pragma once

#include "base.h"

#define MAX_WRITE_NUM	1024

class IWriteFile
{
public:
	virtual ~IWriteFile() { }

public:
	virtual uint32_t write(const void* buffer, uint32_t sizeToWrite) = 0;
	virtual uint32_t writeText( const char* buffer, uint32_t len = MAX_WRITE_NUM ) = 0;
	virtual uint32_t writeLine(const char* text) = 0;
	virtual bool flush() = 0;
	virtual bool seek(int32_t finalPos, bool relativeMovement = false) = 0;
	virtual uint32_t getSize() const = 0;
	virtual int32_t getPos() const = 0;
	virtual const char* getFileName() const = 0;
	virtual bool isOpen() const = 0;
	virtual bool isBinary() const = 0;
};