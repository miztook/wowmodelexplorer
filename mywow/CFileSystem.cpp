#include "stdafx.h"
#include "CFileSystem.h"
#include "CReadFile.h"
#include "CWriteFile.h"

#define RESOURCE_DIR	"\\..\\Data\\"

#define  SUBDIR_SHADER		"Shaders\\"
#define  SUBDIR_FONT		"Font\\"

CFileSystem::CFileSystem()
{
	const c8* workdir = getWorkingDirectory();
	strcpy_s(InitialWorkingDirectory, MAX_PATH, workdir);
	strcpy_s(BaseDirectory, MAX_PATH, workdir);
	strcat_s(BaseDirectory, MAX_PATH, RESOURCE_DIR);

	strcpy_s(ShaderBaseDirectory, MAX_PATH, BaseDirectory);
	strcat_s(ShaderBaseDirectory, MAX_PATH, SUBDIR_SHADER);

	strcpy_s(FontBaseDirectory, MAX_PATH, BaseDirectory);
	strcat_s(FontBaseDirectory, MAX_PATH, SUBDIR_FONT);
}

CFileSystem::~CFileSystem()
{

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

const c8* CFileSystem::getWorkingDirectory()
{
	_getcwd(WorkingDirectory, MAX_PATH);
	return WorkingDirectory;
}

bool CFileSystem::changeWorkingDirectoryTo( const c8* newDirectory )
{
	int res = _chdir(newDirectory);
	if( res )
		strcpy_s( WorkingDirectory, MAX_PATH, newDirectory );
	return res!=0;
}

void CFileSystem::resetWorkingDirectory()
{
	changeWorkingDirectoryTo(InitialWorkingDirectory);
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