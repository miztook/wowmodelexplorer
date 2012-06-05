#pragma once

#include "core.h"

class MPQArchive
{
	bool ok;
public:
	MPQArchive(const c8* filename);
	~MPQArchive();
	void close();
	mpq_archive		mpq_ar;
	bool isPatch;
	bool isLocale;
};

class MPQFile
{
public:
	MPQFile( u8* buf, u32 size, const c8* fname) : buffer(buf), size(size), pointer(0), eof(false)
	{
		strcpy_s(filename, MAX_PATH, fname);
	}
	~MPQFile()
	{
		close();
	}

	u32		read(void* dest, u32 bytes);
	u32		getSize() { return size; }
	u32		getPos() { return pointer; }
	u8*		getBuffer() { return buffer; }
	u8*		getPointer() { return buffer + pointer; }
	bool		isEof() { return eof; }
	void		seek(s32 offset);
	void		seekRelative(s32 offset);
	void		close();
	void		save(const c8* filename);
	const c8*	getFileName() const { return filename; }			

private:
	bool eof;
	u8* buffer;
	u32 pointer, size;
	c8	filename[MAX_PATH];
};

inline void flipcc(char *fcc)
{
	char t;
	t=fcc[0];
	fcc[0]=fcc[3];
	fcc[3]=t;
	t=fcc[1];
	fcc[1]=fcc[2];
	fcc[2]=t;
}
