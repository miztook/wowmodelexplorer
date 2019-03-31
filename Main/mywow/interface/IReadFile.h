#pragma once

#include "base.h"

#define	MAX_READ_NUM		1024

class IReadFile
{
public:
	virtual ~IReadFile() { }

public:
	virtual uint32_t read(void* buffer, uint32_t sizeToRead) = 0;
	virtual uint32_t readText(char* buffer, uint32_t len = MAX_READ_NUM ) = 0;
	virtual uint32_t readLine(char* buffer, uint32_t len = MAX_READ_NUM) = 0;
	virtual uint32_t readLineSkipSpace(char* buffer, uint32_t len = MAX_READ_NUM) = 0;
	virtual bool seek(int32_t finalPos, bool relativePos = false) = 0;
	virtual uint32_t getSize() const = 0;
	virtual int32_t getPos() const = 0;
	virtual bool isEof() const = 0;
	virtual const char* getFileName() const= 0;
	virtual bool isOpen() const = 0;
	virtual bool isBinary() const = 0;
};