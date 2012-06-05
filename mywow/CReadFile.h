#include "IReadFile.h"

class CReadFile : public IReadFile
{
public:
	CReadFile(const char* filename, bool binary);
	virtual ~CReadFile();

public:
	virtual s32 read(void* buffer, u32 sizeToRead);
	virtual s32 readLine(c8* buffer, u32 len = MAX_READ_NUM );
	virtual bool seek(long finalPos, bool relativeMovement = false);
	virtual s32 getSize() const { return FileSize; }
	virtual s32 getPos() const;
	virtual const c8* getFileName() const { return FileName; }
	virtual bool isOpen() const { return File != NULL; }
	virtual bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary);

	bool		IsBinary;
	FILE*	File;
	s32		FileSize;
	c8	FileName[MAX_PATH];
};