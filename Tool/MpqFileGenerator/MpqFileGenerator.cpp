// MpqFileGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "mywow.h"
#include "CFileSystem.h"
#include "wowEnvironment.h"

#pragma comment(lib, "mywow.lib")

#define MPQFILES	"mpqfiles/"

IFileSystem* g_fs = NULL;
wowEnvironment* g_wowEnv = NULL;

int nSucceed = 0;
int nFailed = 0;

void readAndSaveFiles(const c8* filename);

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	initGlobal();
	QMem_Init(1, 80, 30);	

	g_fs = new CFileSystem("", "", false);
	g_wowEnv = new wowEnvironment(g_fs, true, false);

	readAndSaveFiles("mpqfilelist.txt");

	delete g_wowEnv;
	delete g_fs;

	QMem_End();
	deleteGlobal();

	printf("file generator completed. %d Succeed, %d Failed\n", nSucceed, nFailed);
	getchar();
	return 0;
}

void readAndSaveFiles( const c8* filename )
{
	IReadFile* rfile = g_fs->createAndOpenFile(filename, false);
	if (!rfile)
	{
		printf("cannot open file %s\n", filename);
		return;
	}

	c8 buffer[1024] = {0};
	c8 name[512] = {0};
	c8 locale[64] = {0};

	u32 len = rfile->readLine(buffer, 1024);
	while(len)
	{
		u32 count = 0;
		while(count < len)
		{
			if (buffer[count] == ',' && count > 0)
			{
				trim(buffer, count, name, 512);
				trim(&buffer[count+1], locale, 64);

				//read from mpq and save (locale folder or not)
				bool mainFolder = atoi(locale) == 0;

				string512 path = g_fs->getDataDirectory();
				if (!mainFolder)
				{
					path.append(g_wowEnv->getLocale());
					path.append("\\");
				}
				path.append(MPQFILES);
				Q_strlwr(name);
				path.append(name);

				IMemFile* memFile = g_wowEnv->openFile(name, true);

				if (memFile && memFile->save(path.c_str()))
				{
					++nSucceed;
					printf("open and save %s succeed!\n", name);
				}
				else
				{
					++nFailed;
					if(!memFile)
						printf("open %s failed!\n", name);
					else
						printf("save %s failed, dir: %s\n", name, path.c_str());
				}

				delete memFile;

				break;
			}
			++count;
		}
		memset(buffer, 0, sizeof(buffer));
		memset(name, 0, sizeof(name));
		memset(locale, 0, sizeof(locale));
		len = rfile->readLine(buffer);
	}

	delete rfile;
}