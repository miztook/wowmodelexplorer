#include "stdafx.h"
#include "core.h"
#include "CFileSystem.h"
#include "CReadFile.h"
#include "CWriteFile.h"
#include "CSysGlobal.h"
#include "CSysUtility.h"
#include "CLock.h"

#define RESOURCE_SUBDIR  "Data/"
#define LOG_DIR	"Logs/"
#define  SUBDIR_SHADER		"Shaders/"

CFileSystem::CFileSystem(const c8* baseDir, const c8* logDir, bool ignoreSetting)
	: LogGxFile(NULL_PTR), LogResFile(NULL_PTR), LogSoundFile(NULL_PTR)
{
	::memset(LogString, 0, sizeof(LogString));

	c8 tmp[QMAX_PATH];

	if (Q_strlen(baseDir))
	{
		Q_strcpy(tmp, QMAX_PATH, baseDir);
		normalizeDirName(tmp);
		getAbsolutePath(tmp, BaseDirectory, QMAX_PATH);
	}
	else
	{
		c8		workingDirectory[QMAX_PATH];
		if (!Q_getcwd(workingDirectory, QMAX_PATH))
		{
			ASSERT(false);
			return;
		}
		Q_strcpy(tmp, QMAX_PATH, workingDirectory);
		normalizeDirName(tmp);
		Q_strcat(tmp, QMAX_PATH, "../../");
		getAbsolutePath(tmp, BaseDirectory, QMAX_PATH);
	}
	normalizeDirName(BaseDirectory);
	
	CSysUtility::outputDebug("base directory: %s", BaseDirectory);
    
	//data dir
	Q_strcpy(tmp, QMAX_PATH, BaseDirectory);
	normalizeDirName(tmp);
	Q_strcat(tmp, QMAX_PATH, RESOURCE_SUBDIR);
	getAbsolutePath(tmp, DataDirectory, QMAX_PATH);
	normalizeDirName(DataDirectory);

	CSysUtility::outputDebug("data directory: %s", DataDirectory);

	//log dir
	if (Q_strlen(logDir))
	{
		Q_strcpy(tmp, QMAX_PATH, logDir);
		normalizeDirName(tmp);
		Q_strcat(tmp, QMAX_PATH, LOG_DIR);
		getAbsolutePath(tmp, LogDirectory, QMAX_PATH);
	}
	else
	{
		Q_strcpy(tmp, QMAX_PATH, BaseDirectory);
		normalizeDirName(tmp);
		Q_strcat(tmp, QMAX_PATH, LOG_DIR);
		getAbsolutePath(tmp, LogDirectory, QMAX_PATH);
	}
	normalizeDirName(LogDirectory);
	
	CSysUtility::outputDebug("log directory: %s", LogDirectory);

	Q_strcpy(tmp, QMAX_PATH, DataDirectory);
	normalizeDirName(tmp);
	Q_strcat(tmp, QMAX_PATH, SUBDIR_SHADER);
	getAbsolutePath(tmp, ShaderBaseDirectory, QMAX_PATH);
	normalizeDirName(ShaderBaseDirectory);

	CSysUtility::outputDebug("shader directory: %s", ShaderBaseDirectory);

	Configs.readBaseSetting(this);
	
	createLogFiles();

	const c8* wowDir = Configs.getSetting(ECT_SETTING, "wowdir");
	writeLog(ELOG_RES, "wowdir: %s", wowDir);

	if (!ignoreSetting && isFileExists(wowDir))
	{
		Q_strcpy(tmp, QMAX_PATH, wowDir);
		normalizeDirName(tmp);
		Q_strcat(tmp, QMAX_PATH, RESOURCE_SUBDIR);
		getAbsolutePath(tmp, MpqDirectory, QMAX_PATH);
	}
	else
	{
		writeLog(ELOG_RES, "wowdir is not exist: %s", wowDir);
		Q_strcpy(MpqDirectory, QMAX_PATH, DataDirectory);
	}
	normalizeDirName(MpqDirectory);

	CSysUtility::outputDebug("mpq directory: %s", MpqDirectory);
	writeLog(ELOG_RES, "mpq directory: %s", MpqDirectory);
}

CFileSystem::~CFileSystem()
{
	delete LogSoundFile;
	delete LogGxFile;
	delete LogResFile;
}

bool CFileSystem::createDirectory( const c8* dirname )
{
	int ret = Q_mkdir(dirname);

	if (ret == 0 || ret == EEXIST)
		return true;
	return false;
}

bool CFileSystem::deleteDirectory( const c8* dirname )
{
	return Q_rmdir(dirname) == 0;
}

IReadFile* CFileSystem::createAndOpenFile( const c8* filename, bool binary )
{
	CReadFile* file = new CReadFile(filename, binary);
	if (file->isOpen())
		return file;

	delete file;
	return NULL_PTR;
}

IWriteFile* CFileSystem::createAndWriteFile( const c8* filename, bool binary, bool append /*= false*/ )
{
	CWriteFile* file = new CWriteFile(filename, binary, append);
	if (file->isOpen())
		return file;

	delete file;
	return NULL_PTR;
}

void CFileSystem::getAbsolutePath( const c8* filename, c8* outfilename, u32 size )
{
	Q_fullpath(filename, outfilename, size);
}

bool CFileSystem::isFileExists( const c8* filename )
{
	if(strlen(filename) == 0)
		return false;
	return Q_access(filename, 0) == 0;
}

bool CFileSystem::createLogFiles()
{
	if (!isFileExists(LogDirectory))
	{
		if(!createDirectory(LogDirectory))
		{
			ASSERT(false);
			return false;
		}
	}
	//resource log
	{
		string_path  path = LogDirectory;
        path.normalizeDir();
		path.append(getLogFileName(ELOG_RES));
		LogResFile = createAndWriteFile(path.c_str(), false, false);

		if (!LogResFile)
		{
			//ASSERT(false);
		}
	}
	//gx log
	{
		string_path  path = LogDirectory;
        path.normalizeDir();
		path.append(getLogFileName(ELOG_GX));
		LogGxFile = createAndWriteFile(path.c_str(), false, false);

		if (!LogGxFile)
		{
			//ASSERT(false);
		}
	}
	//engine log
	{
		string_path  path = LogDirectory;
        path.normalizeDir();
		path.append(getLogFileName(ELOG_SOUND));
		LogSoundFile = createAndWriteFile(path.c_str(), false, false);

		if (!LogSoundFile)
		{
			//ASSERT(false);
		}
	}
	
	return true;
}

const c8* CFileSystem::getLogFileName( E_LOG_TYPE type ) const
{
	switch (type)
	{
	case ELOG_GX:
		return "gx.log";
	case ELOG_RES:
		return "resource.log";
	case ELOG_SOUND:
		return "sound.log";
	default:
		return "";
	}
}

void CFileSystem::writeLog( E_LOG_TYPE type, const c8* format, ... )
{
	CLock lock(&g_Globals.logCS);

	va_list va;
	va_start( va, format );
	Q_vsprintf( LogString, 1024, format, va );
	va_end( va );

// 	time_t ltime;
// 	time( &ltime );
// 	struct tm gmt;
// 	localtime_s(&gmt, &ltime);
// 	c8 timebuf[64];
// 	strftime(timebuf, 64, "%m/%d %H:%M:%S ", &gmt);
	c8 timebuf[64];
	Q_getLocalTime(timebuf, 64);
	
	string1024 text = timebuf;
	text.append(LogString);
	text.append("\n");
	switch (type)
	{
	case ELOG_GX:
		if(LogGxFile)
		{
			CSysUtility::outputDebug("****** gx: %s\n", LogString);

			LogGxFile->writeText(text.c_str(), 1024);
			LogGxFile->flush();
		}
		break;
	case ELOG_RES:
		if(LogResFile)
		{
			CSysUtility::outputDebug("****** res: %s\n", LogString);

			LogResFile->writeText(text.c_str(), 1024);
			LogResFile->flush();
		}
		break;
	case ELOG_SOUND:
		if(LogResFile)
		{
			CSysUtility::outputDebug("****** sound: %s\n", LogString);

			LogSoundFile->writeText(text.c_str(), 1024);
			LogSoundFile->flush();
		}
		break;
	default:
		break;
	}
}


