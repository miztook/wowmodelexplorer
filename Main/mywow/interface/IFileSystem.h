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
	virtual IReadFile* createAndOpenFile( const char* filename, bool binary ) = 0;
	virtual IWriteFile* createAndWriteFile( const char* filename, bool binary, bool append = false) = 0;

	virtual bool createDirectory(const char* dirname) = 0;
	virtual bool deleteDirectory(const char* dirname) = 0;

	virtual void getAbsolutePath(const char* filename, char* outfilename, uint32_t size ) = 0;

	virtual const char* getBaseDirectory() const = 0;
	virtual const char* getDataDirectory() const = 0;
	virtual const char* getMpqDirectory() const = 0;
	virtual bool isFileExists(const char* filename) = 0;
	virtual const char* getShaderBaseDirectory() const = 0;

	virtual void writeLog(E_LOG_TYPE type, const char* format, ...) = 0;
	virtual IConfigs* getConfigs() const = 0;
};