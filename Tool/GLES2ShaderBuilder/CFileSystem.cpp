#include "stdafx.h"
#include "core.h"
#include "CFileSystem.h"
#include "CReadFile.h"
#include "CWriteFile.h"
#include "CMutex.h"

#define RESOURCE_SUBDIR  "\\Data\\"
#define RESOURCE_DIR	"\\..\\Data\\"
#define LOG_DIR	"\\..\\Logs\\"
#define  SUBDIR_SHADER		"Shaders\\"

CFileSystem::CFileSystem()
	: LogGxFile(NULL), LogResFile(NULL), LogSoundFile(NULL)
{
	::memset(LogString, 0, sizeof(LogString));
	::memset(WorkingDirectory, 0, sizeof(WorkingDirectory));

	if (!_getcwd(InitialWorkingDirectory, MAX_PATH))
	{
		_ASSERT(false);
		return;
	}

	c8 tmp[MAX_PATH];

	strcpy_s(tmp, MAX_PATH, InitialWorkingDirectory);
	strcat_s(tmp, MAX_PATH, RESOURCE_DIR);
	getAbsolutePath(tmp, BaseDirectory, MAX_PATH);

	strcpy_s(tmp, MAX_PATH, BaseDirectory);
	strcat_s(tmp, MAX_PATH, LOG_DIR);
	getAbsolutePath(tmp, LogDirectory, MAX_PATH);

	strcpy_s(tmp, MAX_PATH, BaseDirectory);
	strcat_s(tmp, MAX_PATH, SUBDIR_SHADER);
	getAbsolutePath(tmp, ShaderBaseDirectory, MAX_PATH);

	Configs.readBaseSetting(this);

	const c8* baseDir = Configs.getSetting(ECT_SETTING, "wowdir");
	if (isFileExists(baseDir))
	{
		strcpy_s(tmp, MAX_PATH, baseDir);
		strcat_s(tmp, MAX_PATH, RESOURCE_SUBDIR);
		getAbsolutePath(tmp, MpqDirectory, MAX_PATH);
	}
	else
	{
		strcpy_s(MpqDirectory, MAX_PATH, BaseDirectory);
	}

	createLogFiles();
}

CFileSystem::~CFileSystem()
{
	delete LogSoundFile;
	delete LogGxFile;
	delete LogResFile;
}

bool CFileSystem::createDirectory( const c8* dirname )
{
	int ret = _mkdir(dirname);
	if (ret == 0 || ret == EEXIST)
		return true;
	return false;
}

bool CFileSystem::deleteDirectory( const c8* dirname )
{
	return (_rmdir(dirname) == 0);
}

IReadFile* CFileSystem::createAndOpenFile( const c8* filename, bool binary )
{
	CReadFile* file = new CReadFile(filename, binary);
	if (file->isOpen())
		return file;

	delete file;
	return NULL;
}

IWriteFile* CFileSystem::createAndWriteFile( const c8* filename, bool binary, bool append /*= false*/ )
{
	CWriteFile* file = new CWriteFile(filename, binary, append);
	if (file->isOpen())
		return file;

	delete file;
	return NULL;
}

void CFileSystem::getAbsolutePath( const c8* filename, c8* outfilename, u32 size )
{
	if (!_fullpath(outfilename, filename, size))	
		_ASSERT(false);
}

bool CFileSystem::isFileExists( const c8* filename )
{
	return _access_s(filename, 0) == 0;
}

bool CFileSystem::createLogFiles()
{
	if (!isFileExists(LogDirectory))
	{
		if(!createDirectory(LogDirectory))
		{
			_ASSERT(false);
			return false;
		}
	}
	//resource log
	{
		string256  path = LogDirectory;
		path.append(getLogFileName(ELOG_RES));
		LogResFile = createAndWriteFile(path.c_str(), false, false);

		if (!LogResFile)
		{
			_ASSERT(false);
		}
	}
	//gx log
	{
		string256  path = LogDirectory;
		path.append(getLogFileName(ELOG_GX));
		LogGxFile = createAndWriteFile(path.c_str(), false, false);

		if (!LogGxFile)
		{
			_ASSERT(false);
		}
	}
	//engine log
	{
		string256  path = LogDirectory;
		path.append(getLogFileName(ELOG_SOUND));
		LogSoundFile = createAndWriteFile(path.c_str(), false, false);

		if (!LogSoundFile)
		{
			_ASSERT(false);
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
	va_list va;
	va_start( va, format );
	vsprintf_s( LogString, 1024, format, va );
	va_end( va );

// 	time_t ltime;
// 	time( &ltime );
// 	struct tm gmt;
// 	localtime_s(&gmt, &ltime);
// 	c8 timebuf[64];
// 	strftime(timebuf, 64, "%m/%d %H:%M:%S ", &gmt);
	c8 timebuf[64];
	SYSTEMTIME time;
	GetLocalTime(&time);
	sprintf_s(timebuf, 64, "%0.2d/%0.2d %0.2d:%0.2d:%0.2d.%0.3d\t", time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	string1024 text = timebuf;
	text.append(LogString);
	text.append("\n");
	switch (type)
	{
	case ELOG_GX:
		if(LogGxFile)
		{
			LogGxFile->writeText(text.c_str(), 1024);
			LogGxFile->flush();
		}
		break;
	case ELOG_RES:
		if(LogResFile)
		{
			LogResFile->writeText(text.c_str(), 1024);
			LogResFile->flush();
		}
		break;
	case ELOG_SOUND:
		if(LogResFile)
		{
			LogSoundFile->writeText(text.c_str(), 1024);
			LogSoundFile->flush();
		}
		break;
	default:
		break;
	}
}

bool CFileSystem::iterateFiles( const c8* dirname, const c8* ext, ITERATEFILECALLBACK callback, void* args )
{
	c8 path[MAX_PATH];
	strcpy_s(path, MAX_PATH, dirname);
	c8 last = path[strlen(path) -1];
	if (last != '/' && last != '\\' )
	{
		strcat_s(path, MAX_PATH, "\\");
	}
	strcat_s(path, MAX_PATH, ext);

	_finddata_t finddata;
	intptr_t hfile = _findfirst(path, &finddata);
	if (hfile == -1)
		return false;

	do
	{
		if (strcmp(finddata.name, "..") == 0 || strcmp(finddata.name, ".") == 0)
			continue;

		if (finddata.attrib & (_A_HIDDEN | _A_SYSTEM))
			continue;

		c8 subpath[MAX_PATH];
		strcpy_s(subpath, dirname);
		last = subpath[strlen(subpath) -1];
		if (last != '/' && last != '\\' )
		{
			strcat_s(subpath, MAX_PATH, "\\");
		}
		strcat_s(subpath, finddata.name);

		if (finddata.attrib & _A_SUBDIR)
		{	
			iterateFiles(subpath, ext, callback, args);
		}
		else
		{
			callback(subpath, args);
		}
	} while (_findnext(hfile, &finddata) != -1);

	if (errno != ENOENT)
	{
		_findclose(hfile);
		return false;
	}

	_findclose(hfile);
	return true;
}

