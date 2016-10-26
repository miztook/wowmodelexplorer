#pragma once

#include "base.h"

class IMemFile
{
public:
	virtual ~IMemFile() {}

public:
	virtual u32 read(void* dest, u32 bytes) = 0;
	virtual u32 getSize() const = 0;
	virtual u32 getPos() const = 0;
	virtual u8* getBuffer() = 0;
	virtual u8* getPointer() = 0;
	virtual bool isEof() const = 0;
	virtual bool seek(s32 offset, bool relative=false) = 0;
	virtual void close() = 0;
	virtual bool save(const c8* filename) = 0;
	virtual const c8*	getFileName() const = 0;	
};