#pragma once

#include "base.h"
#include "IFileSystem.h"
#include "CConfigs.h"

class CFileSystem : public IFileSystem
{
private:
	DISALLOW_COPY_AND_ASSIGN(CFileSystem);

public:
	CFileSystem(const c8* baseDir, const c8* logDir, bool ignoreSetting);
	~CFileSystem();

public:
	virtual IReadFile* createAndOpenFile( const c8* filename, bool binary );
	virtual IWriteFile* createAndWriteFile( const c8* filename, bool binary, bool append = false);

	virtual bool createDirectory(const c8* dirname);
	virtual bool deleteDirectory(const c8* dirname);

	virtual void getAbsolutePath(const c8* filename, c8* outfilename, u32 size );

	virtual const c8* getBaseDirectory() const { return BaseDirectory; }
	virtual const c8* getDataDirectory() const { return DataDirectory; }
	virtual const c8* getMpqDirectory() const { return MpqDirectory; }
	virtual bool isFileExists(const c8* filename);
	virtual const c8* getShaderBaseDirectory() const { return ShaderBaseDirectory; }

	virtual void writeLog(E_LOG_TYPE type, const c8* format, ...);

	virtual IConfigs* getConfigs() const { return (IConfigs*)&Configs; }

private:
	bool createLogFiles();

	const c8* getLogFileName(E_LOG_TYPE type) const;

private:
	c8		BaseDirectory[QMAX_PATH];
	c8		DataDirectory[QMAX_PATH];
	c8		MpqDirectory[QMAX_PATH];
	c8		ShaderBaseDirectory[QMAX_PATH];
	c8		LogDirectory[QMAX_PATH];

	CConfigs		Configs;

	IWriteFile* LogGxFile;
	IWriteFile* LogResFile;
	IWriteFile* LogSoundFile;

	c8		LogString[1024];
};