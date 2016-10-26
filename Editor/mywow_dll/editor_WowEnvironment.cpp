#include "stdafx.h"
#include "editor_WowEnvironment.h"

std::vector<string_cs256>		g_vFileNames;
std::vector<string_cs256>		g_vDirFullNames;

bool WowEnvironment_Exists( const c8* filename )
{
	return g_Engine->getWowEnvironment()->exists(filename);
}

u32 WowEnvironment_GetCascFileCount()
{
	return g_Engine->getWowEnvironment()->getCascFileCount();
}

const c8* WowEnvironment_GetCascFile(int index)
{
	return g_Engine->getWowEnvironment()->getCascFile(index);
}

void WowEnvironment_ClearOwnCascFiles()
{
	g_Engine->getWowEnvironment()->clearOwnCascFiles();
}

void WowEnvironment_AddOwnCascFile(const c8* filename)
{
	g_Engine->getWowEnvironment()->addOwnCascFile(filename);
}

void WowEnvironment_FinishOwnCascFiles()
{
	g_Engine->getWowEnvironment()->finishOwnCascFiles();
}

int WowEnvironment_GetFileCount_InDir(const c8* baseDir, const c8* ext, bool useOwn)
{
	g_Engine->getWowEnvironment()->getFiles(baseDir, ext, g_vFileNames, useOwn);
	return (int)g_vFileNames.size();
}

void WowEnvironment_GetFile_InDir(bool fullname, int index, c8* outfilename, u32 size)
{
	if (index < 0 || index >= (int)g_vFileNames.size())
	{
		Q_strcpy(outfilename, size, "");
	}
	else
	{
		if(fullname)
			Q_strcpy(outfilename, size, g_vFileNames[index].c_str());
		else
			getFileNameA(g_vFileNames[index].c_str(), outfilename, size);
	}
}

int WowEnvironment_GetDirectoryCount_InDir(const c8* baseDir, bool useOwn)
{
	g_Engine->getWowEnvironment()->getDirectories(baseDir, g_vDirFullNames, useOwn);
	return (int)g_vDirFullNames.size();
}

void WowEnvironment_GetDirectory_InDir(bool fullname, int index, c8* outdirname, u32 size)
{
	if (fullname)
	{
		if (index < 0 || index >= (int)g_vDirFullNames.size())
			Q_strcpy(outdirname, size, "");
		else
			Q_strcpy(outdirname, size, g_vDirFullNames[index].c_str());
	}
	else
	{
		if (index < 0 || index >= (int)g_vDirFullNames.size())
			Q_strcpy(outdirname, size, "");
		else
			getFileNameA(g_vDirFullNames[index].c_str(), outdirname, size);
	}
}

const c8* WowEnvironment_GetLocale()
{
	return g_Engine->getWowEnvironment()->getLocale();
}

void WowEnvironment_Clear()
{
	std::vector<string_cs256>().swap(g_vFileNames);
	std::vector<string_cs256>().swap(g_vDirFullNames);
}
