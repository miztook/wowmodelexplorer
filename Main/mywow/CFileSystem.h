#pragma once

#include "base.h"
#include "IFileSystem.h"
#include "CConfigs.h"

class CFileSystem : public IFileSystem
{
private:
	DISALLOW_COPY_AND_ASSIGN(CFileSystem);

public:
	CFileSystem(const char* baseDir, const char* logDir, bool ignoreSetting);
	~CFileSystem();

public:
	virtual IReadFile* createAndOpenFile( const char* filename, bool binary );
	virtual IWriteFile* createAndWriteFile( const char* filename, bool binary, bool append = false);

	virtual bool createDirectory(const char* dirname);
	virtual bool deleteDirectory(const char* dirname);

	virtual void getAbsolutePath(const char* filename, char* outfilename, uint32_t size );

	virtual const char* getBaseDirectory() const { return BaseDirectory; }
	virtual const char* getDataDirectory() const { return DataDirectory; }
	virtual const char* getMpqDirectory() const { return MpqDirectory; }
	virtual bool isFileExists(const char* filename);
	virtual const char* getShaderBaseDirectory() const { return ShaderBaseDirectory; }

	virtual void writeLog(E_LOG_TYPE type, const char* format, ...);

	virtual IConfigs* getConfigs() const { return (IConfigs*)&Configs; }

private:
	bool createLogFiles();

	const char* getLogFileName(E_LOG_TYPE type) const;

private:
	char		BaseDirectory[QMAX_PATH];
	char		DataDirectory[QMAX_PATH];
	char		MpqDirectory[QMAX_PATH];
	char		ShaderBaseDirectory[QMAX_PATH];
	char		LogDirectory[QMAX_PATH];

	CConfigs		Configs;

	IWriteFile* LogGxFile;
	IWriteFile* LogResFile;

	char		LogString[1024];
};