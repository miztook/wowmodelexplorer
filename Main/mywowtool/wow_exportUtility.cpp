#include "stdafx.h"
#include "wow_exportUtility.h"
#include "TGAImageWriter.h"

bool AUX_CopyFile(const char* dst, const char* src)
{
	char filename[MAX_PATH];
	getFileNameA(src, filename, MAX_PATH);

	string512 strPath;
	strPath = dst;
	strPath.normalizeDir();
	strPath.append(filename);

	::CopyFileA(src, strPath.c_str(), false);

	DWORD flag = GetFileAttributes(strPath.c_str());
	if (flag == -1)
	{
		ASSERT(false);
		return false;
	}

	if (flag & FILE_ATTRIBUTE_READONLY) 
	{
		flag ^= FILE_ATTRIBUTE_READONLY;
		if(!SetFileAttributes(strPath.c_str(), flag))
			return false;
	}

	return true;
}

bool AUX_CreateDirectory(const char* szDir)
{
	string512 strDir = szDir;
	int iLen = strDir.length();

	if (iLen <= 3)
		return true;

	//	Clear the last '\\'
	if (strDir[iLen-1] == '\\')
		strDir[iLen-1] = '\0';

	//	Save current directory
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);

	string512 strTemp;

	int iPos = strDir.findNext('\\', 0);
	while (iPos > 0)
	{
		strDir.subString(0, iPos, strTemp);
		CreateDirectory(strTemp.c_str(), NULL);
		iPos = strDir.findNext('\\', iPos+1);
	}

	CreateDirectory(szDir, NULL);

	//	Restore current directory
	SetCurrentDirectory(szCurDir);

	return true;
}

bool AUX_ExportBlpAsTga(const char* blpfilename, const char* tgafilename, bool bAlpha)
{
	if (bAlpha)
	{
		IImage* image = g_Engine->getResourceLoader()->loadBLPAsImage(blpfilename, false);
		if (!image)
			return false;

		ASSERT(image->getColorFormat() == ECF_A8R8G8B8);
		dimension2du size = image->getDimension();

		bool ret = TGAWriteFile(tgafilename, size.Width, size.Height, TGA_FORMAT_BGRA, image->getData());

		image->drop();
		return true;
	}
	else
	{
		IImage* image = g_Engine->getResourceLoader()->loadBLPAsImage(blpfilename, true);
		if(!image)
			return false;

		ASSERT(image->getColorFormat() == ECF_A8R8G8B8);
		dimension2du size = image->getDimension();

		u32 dataSize = size.Width * size.Height * getBytesPerPixelFromFormat(ECF_R8G8B8);
		u8* data = (u8*)Z_AllocateTempMemory(dataSize);
		image->copyToScaling(data, size.Width, size.Height, ECF_R8G8B8);

		bool ret = TGAWriteFile(tgafilename, size.Width, size.Height, TGA_FORMAT_BGR, data);
		ASSERT(ret);

		Z_FreeTempMemory(data);

		image->drop();
		return true;
	}

	return false;
}

bool AUX_ExportBlpAsTgaDir(const char* blpfilename, const char* dirname, bool bAlpha)
{
	string512 outfilename = dirname;
	outfilename.normalizeDir();

	char filename[DEFAULT_SIZE];
	getFileNameNoExtensionA(blpfilename, filename, DEFAULT_SIZE);
	Q_strcat(filename, DEFAULT_SIZE, ".tga");

	outfilename.append(filename);

	return AUX_ExportBlpAsTga(blpfilename, outfilename.c_str(), bAlpha);
}
