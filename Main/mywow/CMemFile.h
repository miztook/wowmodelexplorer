#pragma once

#include "IMemFile.h"
#include "function.h"

class CMemFile : public IMemFile
{
public:
	CMemFile( uint8_t* buf, uint32_t size, const char* fname, bool tmp) : buffer(buf), size(size), pointer(0), eof(false), temp(tmp)
	{
		Q_strcpy(filename, QMAX_PATH, fname);
	}
	~CMemFile()
	{
		close();
	}

	uint32_t		read(void* dest, uint32_t bytes);
	uint32_t		getSize() const { return size; }
	uint32_t		getPos() const { return pointer; }
	uint8_t*		getBuffer() { return buffer; }
	uint8_t*		getPointer() { return buffer + pointer; }
	bool		isEof() const { return eof; }
	bool		seek(int32_t offset, bool relative=false);
	void		close();
	bool		save(const char* filename);
	const char*	getFileName() const { return filename; }			

private:
	uint8_t*		buffer;
	uint32_t		pointer;
	uint32_t		size;
	char		filename[QMAX_PATH];
	bool temp;
	bool eof;
};