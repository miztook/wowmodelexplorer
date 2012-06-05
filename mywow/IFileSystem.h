#pragma once

#include "base.h"

class IReadFile;
class IWriteFile;

class IFileSystem
{
public:
	virtual ~IFileSystem() {}

public:
	virtual IReadFile* createAndOpenFile( const c8* filename, bool binary ) = 0;
	virtual IWriteFile* createAndWriteFile( const c8* filename, bool binary, bool append = false) = 0;

	virtual const c8* getWorkingDirectory() = 0;
	virtual bool changeWorkingDirectoryTo(const c8* newDirectory) = 0;
	virtual void resetWorkingDirectory() = 0;

	virtual void getAbsolutePath(const c8* filename, c8* outfilename, u32 size ) = 0;

	virtual const c8* getBaseDirectory() const = 0;
	virtual bool isFileExists(const c8* filename) = 0;
	virtual const c8* getShaderBaseDirectory() const = 0;
	virtual const c8* getFontBaseDirectory() const  = 0;
};