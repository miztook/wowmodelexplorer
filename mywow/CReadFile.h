#include "IReadFile.h"

class CReadFile : public IReadFile
{
private:
	DISALLOW_COPY_AND_ASSIGN(CReadFile);

public:
	CReadFile(const char* filename, bool binary);
	virtual ~CReadFile();

public:
	virtual u32 read(void* buffer, u32 sizeToRead);
	virtual u32 readLine(c8* buffer, u32 len = MAX_READ_NUM );
	virtual bool seek(s32 finalPos, bool relativePos = false);
	virtual u32 getSize() const { return FileSize; }
	virtual s32 getPos() const;
	virtual bool isEof() const;
	virtual const c8* getFileName() const { return FileName; }
	virtual bool isOpen() const { return File != NULL; }
	virtual bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary);

	bool		IsBinary;
	FILE*		File;
	u32		FileSize;
	c8	FileName[MAX_PATH];
};