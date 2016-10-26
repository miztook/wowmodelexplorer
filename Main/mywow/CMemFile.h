#pragma once

#include "IMemFile.h"
#include "function.h"

class CMemFile : public IMemFile
{
public:
	CMemFile( u8* buf, u32 size, const c8* fname, bool tmp) : buffer(buf), size(size), pointer(0), eof(false), temp(tmp)
	{
		Q_strcpy(filename, QMAX_PATH, fname);
	}
	~CMemFile()
	{
		close();
	}

	u32		read(void* dest, u32 bytes);
	u32		getSize() const { return size; }
	u32		getPos() const { return pointer; }
	u8*		getBuffer() { return buffer; }
	u8*		getPointer() { return buffer + pointer; }
	bool		isEof() const { return eof; }
	bool		seek(s32 offset, bool relative=false);
	void		close();
	bool		save(const c8* filename);
	const c8*	getFileName() const { return filename; }			

private:
	u8*		buffer;
	u32		pointer;
	u32		size;
	c8		filename[QMAX_PATH];
	bool temp;
	bool eof;
};