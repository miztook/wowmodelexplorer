#pragma once

#include "base.h"

class IMemFile
{
public:
	virtual ~IMemFile() {}

public:
	virtual uint32_t read(void* dest, uint32_t bytes) = 0;
	virtual uint32_t getSize() const = 0;
	virtual uint32_t getPos() const = 0;
	virtual uint8_t* getBuffer() = 0;
	virtual uint8_t* getPointer() = 0;
	virtual bool isEof() const = 0;
	virtual bool seek(int32_t offset, bool relative=false) = 0;
	virtual void close() = 0;
	virtual bool save(const char* filename) = 0;
	virtual const char*	getFileName() const = 0;	
};