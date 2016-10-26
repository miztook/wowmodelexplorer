// BlpConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "mywow.h"
#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "CBLPImage.h"

#include "baseDefine.h"
#include "BlpToPVR.h"
#include "BlpToKTX.h"

#ifdef AUTOMATIC_LINK
#pragma comment(lib, "PVRTexLib.lib")
#pragma comment(lib, "mywow.lib")
//#pragma comment(linker,"/nodefaultlib:libcmt.lib")
#endif

#define MPQFILES "mpqfiles/"

#define VSPATH "Shaders/Vertex/gles2_vs/"
#define PSPATH "Shaders/Pixel/gles2_ps/"

#define OUTPUTPATH_IOS	"D:/work/iosbundle/Data"
#define OUTPUTPATH_ANDROID	"D:/work/androidbundle/Data"

IFileSystem* g_fs = NULL;
wowEnvironment* g_wowEnv = NULL;
c8 g_szOuputPath[MAX_PATH];

int nTexSucceed = 0;
int nTexFailed = 0;
int nNonTexSucceed = 0;
int nNonTexFailed = 0;

void convertBlpToPVR();
void convertBlpToKTX();
void copyShaders();

void callbackBlpToPVR(const c8* filename, void* param);
void callbackBlpToKTX(const c8* filename, void* param);

void callbackNonTexFile(const c8* filename, void* param);

void callbackShaderFile(const c8* filename, void* param);

CBlpToPVR g_BlpToPVR(PVRCQ_Normal);
CBlpToKTX g_BlpToKTX(PVRCQ_Normal);

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	initGlobal();
	QMem_Init(1, 30, 30);	

	g_fs = new CFileSystem("", "", true);
	g_wowEnv = new wowEnvironment(g_fs, false, false);

	bool bPvr = true;
	if (bPvr)
		strcpy_s(g_szOuputPath, MAX_PATH, OUTPUTPATH_IOS);
	else
		strcpy_s(g_szOuputPath, MAX_PATH, OUTPUTPATH_ANDROID);

	if (bPvr)
		convertBlpToPVR();					//×ª»»ÌùÍ¼£¬¿½±´ÎÄ¼þ
	else
		convertBlpToKTX();

	copyShaders();					//¿½±´gles shaders

	delete g_wowEnv;
	delete g_fs;

	QMem_End();
	deleteGlobal();

	printf("\nblp converter completed\nTextures: %d Succeed, %d Failed\nNon Textures: %d Succeed, %d Failed\n", 
		nTexSucceed, nTexFailed, nNonTexSucceed, nNonTexFailed);
	getchar();
	return 0;
}

void copyShaders()
{
	printf("Copy GLES2 shaders... start at %s ...\n\n", g_fs->getMpqDirectory());
	
	string256 path = g_fs->getMpqDirectory();
	path.append(VSPATH);
	Q_iterateFiles(path.c_str(), "*.*", callbackShaderFile, NULL, g_fs->getMpqDirectory());

	path = g_fs->getMpqDirectory();
	path.append(PSPATH);
	Q_iterateFiles(path.c_str(), "*.*", callbackShaderFile, NULL, g_fs->getMpqDirectory());
}

void callbackShaderFile( const c8* filename, void* param )
{
	if (!hasFileExtensionA(filename, "fx"))
		return;

	string256 path = g_fs->getMpqDirectory();
	path.append(filename);

	string256 outpath = g_szOuputPath;
	outpath.normalizeDir();
	outpath.append(filename);

	//make outpath directory exist
	if(!Q_MakeDirForFileName(outpath.c_str()))
	{
		printf("cannot create dir %s\n", outpath.c_str());
		return;
	}

	if(FALSE == CopyFileA(path.c_str(), outpath.c_str(), FALSE))
	{
		printf("cannot save file %s\n\n", outpath.c_str());
	}
	else
	{
		printf("successfully save file %s \n\n", outpath.c_str());
	}
}

void convertBlpToPVR()
{
	printf("PVR transfer... start at %s ...\n\n", g_fs->getMpqDirectory());
	g_wowEnv->iterateFiles("*.*", callbackBlpToPVR, NULL);
}

void callbackBlpToPVR( const c8* filename, void* param )
{
	if (!hasFileExtensionA(filename, "blp"))
	{
		callbackNonTexFile(filename, param);
		return;
	}

	c8 shortfilename[MAX_PATH];
	const c8* start = strstr(filename, MPQFILES);
	u32 len = strlen(MPQFILES);
	Q_strcpy(shortfilename, MAX_PATH, start ? start + len : filename);

	IMemFile* file = g_wowEnv->openFile(shortfilename);
	if (!file)
	{
		printf("failed, cannot load blp %s\n", shortfilename);
		++nTexFailed;

		return;
	}

	IBLPImage* image = new CBLPImage();
	if (!image->loadFile(file, false))
	{
		printf("failed, cannot load blp %s\n", shortfilename);
		++nTexFailed;
		delete file;
		image->drop();

		return;
	}

	//convert
	c8 path[256];
	getFullFileNameNoExtensionA(filename, path, 256);
	strcat_s(path, 256, ".pvr");

	string256 outpath;
	outpath = g_szOuputPath;
	outpath.normalizeDir();
	outpath.append(path);

	//make outpath directory exist
	if(!Q_MakeDirForFileName(outpath.c_str()))
	{
		printf("cannot create dir %s\n", outpath.c_str());
		delete file;
		image->drop();
		return;
	}

	printf("start convert pvr texture: %s\n", shortfilename);
	if (!g_BlpToPVR.convertBlpToPVR(image, outpath.c_str()))
	{
		++nTexFailed;
		printf("%s\n\n", g_BlpToPVR.getLastErrorString());
	}
	else
	{
		++nTexSucceed;
		printf("successfully save file %s \n\n", outpath.c_str());
	}

	delete file;
	image->drop();
}

void convertBlpToKTX( )
{
	printf("KTX transfer... start at %s ...\n\n", g_fs->getMpqDirectory());
	g_wowEnv->iterateFiles("*.*", callbackBlpToKTX, NULL);
}

void callbackBlpToKTX( const c8* filename, void* param )
{
	if (!hasFileExtensionA(filename, "blp"))
	{
		callbackNonTexFile(filename, param);
		return;
	}

	c8 shortfilename[MAX_PATH];
	const c8* start = strstr(filename, MPQFILES);
	u32 len = strlen(MPQFILES);
	Q_strcpy(shortfilename, MAX_PATH, start ? start + len : filename);

	IMemFile* file = g_wowEnv->openFile(shortfilename);
	if (!file)
	{
		printf("failed, cannot load blp %s\n", shortfilename);
		++nTexFailed;

		return;
	}

	IBLPImage* image = new CBLPImage();
	if (!image->loadFile(file, false))
	{
		printf("failed, cannot load blp %s\n", shortfilename);
		++nTexFailed;
		delete file;
		image->drop();

		return;
	}

	//convert
	c8 path[256];
	getFullFileNameNoExtensionA(filename, path, 256);
	strcat_s(path, 256, ".ktx");

	string256 outpath;
	outpath = g_szOuputPath;
	outpath.normalizeDir();
	outpath.append(path);

	//make outpath directory exist
	if(!Q_MakeDirForFileName(outpath.c_str()))
	{
		printf("cannot create dir %s\n", outpath.c_str());
		delete file;
		image->drop();
		return;
	}

	printf("start convert ktx texture: %s\n", shortfilename);
	if (!g_BlpToKTX.convertBlpToKTX(image, outpath.c_str()))
	{
		++nTexFailed;
		printf("%s\n\n", g_BlpToKTX.getLastErrorString());
	}
	else
	{
		++nTexSucceed;
		printf("successfully save file %s \n\n", outpath.c_str());
	}

	delete file;
	image->drop();
}

void callbackNonTexFile( const c8* filename, void* param )
{
	if (hasFileExtensionA(filename, "blp") || 
		hasFileExtensionA(filename, "pvr") || 
		hasFileExtensionA(filename, "ktx") ||
		hasFileExtensionA(filename, "tga"))
		return;

	c8 shortfilename[MAX_PATH];
	const c8* start = strstr(filename, MPQFILES);
	u32 len = strlen(MPQFILES);
	Q_strcpy(shortfilename, MAX_PATH, start ? start + len : filename);

	IMemFile* file = g_wowEnv->openFile(shortfilename);
	if (!file)
	{
		printf("failed, cannot load file %s\n", shortfilename);
		++nNonTexFailed;
		return;
	}

	string256 outpath;
	outpath = g_szOuputPath;
	outpath.normalizeDir();
	outpath.append(filename);

	//make outpath directory exist
	if(!file->save(outpath.c_str()))
	{
		printf("cannot save file %s\n\n", outpath.c_str());
		++nNonTexFailed;
	}
	else
	{
		printf("successfully save file %s \n\n", outpath.c_str());
		++nNonTexSucceed;
	}
	
	delete file;
}
