#include "StdAfx.h"
#include "utility.h"

#define MAX_PATH 260

bool iterateFiles( const char* dirname, const char* ext, ITERATEFILECALLBACK callback, void* args )
{
	char path[MAX_PATH];
	strcpy_s(path, MAX_PATH, dirname);
	char last = path[strlen(path) -1];
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

		if (finddata.attrib & _A_SUBDIR)
		{
			char subpath[MAX_PATH];
			strcpy_s(subpath, dirname);
			last = subpath[strlen(subpath) -1];
			if (last != '/' && last != '\\' )
			{
				strcat_s(subpath, MAX_PATH, "\\");
			}
			strcat_s(subpath, finddata.name);
			iterateFiles(subpath, ext, callback, args);
		}
		else
		{
			callback(finddata.name, args);
		}
	} while (_findnext(hfile, &finddata) != -1);

	if (errno != ENOENT)
	{
		_findclose(hfile);
		return false;
	}

	_findclose(hfile);
	return false;
}

void getFileNameNoExtensionA( const char* filename, char* outfilename, size_t size )
{
	const char* lastSlash = strrchr( filename, '/' );
	const char* lastBackSlash = strrchr( filename, '\\' );
	if ( lastSlash < lastBackSlash )
		lastSlash = lastBackSlash;
	const char* p = strrchr(filename, '.' );

	bool noPrefix = (!lastSlash || *(lastSlash+1) == '\0');
	bool noSuffix = (!p || *(p+1)=='\0');
	if (noPrefix && noSuffix)
		strcpy_s(outfilename, size, filename);
	else if(noPrefix)			//suffix
		strncpy_s(outfilename, size, filename, p-filename);
	else if(noSuffix)			//prefix
		strcpy_s( outfilename, size, lastSlash+1 );
	else
		strncpy_s(outfilename, size, lastSlash+1, p-(lastSlash+1));
}

void getFileExtensionA( const char* filename, char* outfilename, size_t size )
{
	const char* p = strrchr(filename, '.' );
	if (!p || *(p+1)=='\0')
		strcpy_s(outfilename, size, "");
	else
		strcpy_s( outfilename, size, p+1 );
}

