#pragma once

#include "base.h"
#include "IFileSystem.h"

class CFileSystem : public IFileSystem
{
private:
	DISALLOW_COPY_AND_ASSIGN(CFileSystem);

public:
	CFileSystem();
	~CFileSystem();

public:
	virtual IReadFile* createAndOpenFile( const c8* filename, bool binary );
	virtual IWriteFile* createAndWriteFile( const c8* filename, bool binary, bool append = false);

	virtual const c8* getWorkingDirectory();
	virtual bool changeWorkingDirectoryTo(const c8* newDirectory);
	virtual void resetWorkingDirectory();

	virtual void getAbsolutePath(const c8* filename, c8* outfilename, u32 size );

	virtual const c8* getBaseDirectory() const { return BaseDirectory; }
	virtual bool isFileExists(const c8* filename);
	virtual const c8* getShaderBaseDirectory() const { return ShaderBaseDirectory; }
	virtual const c8* getFontBaseDirectory() const  { return FontBaseDirectory; }

private:
	c8		WorkingDirectory[MAX_PATH];
	c8		InitialWorkingDirectory[MAX_PATH];
	c8		BaseDirectory[MAX_PATH];
	c8		ShaderBaseDirectory[MAX_PATH];
	c8		FontBaseDirectory[MAX_PATH];
};