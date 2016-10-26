#pragma once

#include "base.h"

class IReadFile;
class IWriteFile;
class IConfigs;

class IFileSystem
{
public:
	virtual ~IFileSystem() {}

public:
	virtual IReadFile* createAndOpenFile( const c8* filename, bool binary ) = 0;
	virtual IWriteFile* createAndWriteFile( const c8* filename, bool binary, bool append = false) = 0;

	virtual bool createDirectory(const c8* dirname) = 0;
	virtual bool deleteDirectory(const c8* dirname) = 0;

	virtual void getAbsolutePath(const c8* filename, c8* outfilename, u32 size ) = 0;

	virtual const c8* getBaseDirectory() const = 0;
	virtual const c8* getDataDirectory() const = 0;
	virtual const c8* getMpqDirectory() const = 0;
	virtual bool isFileExists(const c8* filename) = 0;
	virtual const c8* getShaderBaseDirectory() const = 0;

	virtual void writeLog(E_LOG_TYPE type, const c8* format, ...) = 0;
	virtual IConfigs* getConfigs() const = 0;
};