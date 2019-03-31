#include "stdafx.h"
#include "wowEnvironment.h"
#include "mpq_stormlib.h"
#include "mywow.h"
#include "CMemFile.h"

#define MPQFILES	"mpqfiles/"

#if WOW_VER == 80
#define LISTFILE	"listfile80.txt"
#elif WOW_VER == 70
#define LISTFILE	"listfile70.txt"
#elif WOW_VER == 60
#define LISTFILE    "listfile60.txt"
#endif

const char* basempqfiles[] = 
{
	"alternate.mpq",
	"model.mpq",
	"itemtexture.mpq",
	"texture.mpq",
	"interface.mpq",
	"expansion1.mpq",
	"expansion2.mpq",
	"expansion3.mpq",
	"expansion4.mpq",
	"sound.mpq",
	"world.mpq",
	"misc.mpq",
};

const char* localempqfiles[] =
{
	"locale",
};

const char* updatefiles[] =
{
	"16016",
	"16048",
	"16057",
	"16309",
	"16357",
	"16516",
	"16650",
	"16844",
	"16965",
	"17116",
	"17266",
	"17325",
	"17345",
	"17538",
	"17645",
	"17688",
	"17898",
	"18273",
};

wowEnvironment::wowEnvironment(IFileSystem* fs, bool useCompress, bool outputFilename)
	: FileSystem(fs), UseLocale(true), RecordFile(nullptr), hStorage(nullptr), CascLocale(0)
{
#if defined(MW_USE_MPQ) || defined(MW_USE_CASC)
	UseCompress = useCompress;
#else
	UseCompress = false;
#endif

	Locale = FileSystem->getConfigs()->getSetting(ECT_SETTING, "locale");
	if (Locale.empty())
	{
		Locale = "zhCN";
	}

	getCascLocale();

	Q_strcpy(LocalePath, QMAX_PATH, FileSystem->getMpqDirectory());
	Q_strcat(LocalePath, QMAX_PATH, Locale.c_str());
	Q_strcat(LocalePath, QMAX_PATH, "\\");
    normalizeDirName(LocalePath);

    CSysUtility::outputDebug(LocalePath);
    
#if WOW_VER < 60
	if (!FileSystem->isFileExists(LocalePath))
	{
		Locale = "enUS";
		Q_strcpy(LocalePath, QMAX_PATH, FileSystem->getMpqDirectory());
		Q_strcat(LocalePath, QMAX_PATH, Locale.c_str());
		Q_strcat(LocalePath, QMAX_PATH, "\\");
        normalizeDirName(LocalePath);
	}

	UseAlternate = Q_stricmp("true", FileSystem->getConfigs()->getSetting(ECT_SETTING, "alternate")) == 0;
#else
	UseAlternate = false;
#endif

#if WOW_VER < 60
	if (!FileSystem->isFileExists(LocalePath))
	{
		UseLocale = false;
		FileSystem->writeLog(ELOG_RES, "cannot find locale: %s, use mpq: %s, alternate: %s", 
			Locale.c_str(), 
			UseCompress ? "true" : "false",
			UseAlternate ? "true" : "false");
	}
	else
#endif
	{
		FileSystem->writeLog(ELOG_RES, "current locale: %s, use mpq: %s, alternate: %s",
			Locale.c_str(), 
			UseCompress ? "true" : "false",
			UseAlternate ? "true" : "false");
	}

	InitializeListHead(&LocaleMpqArchiveList);
	InitializeListHead(&MainMpqArchiveList);
	InitializeListHead(&LocalePatchMpqArchiveList);
	InitializeListHead(&MainPatchMpqArchiveList);

	if(UseCompress)
		loadRoot();

	if(outputFilename)
	{
		const char* filename = FileSystem->getConfigs()->getSetting(ECT_SETTING, "output");
		RecordFile = FileSystem->createAndWriteFile(filename, false);
	}

	CascListFiles.reserve(5000);
	OwnCascFiles.reserve(2000);

	loadCascListFiles();
}

wowEnvironment::~wowEnvironment()
{
	DirIndexMap.clear();
	delete RecordFile;

	if(UseCompress)
		unloadRoot();
}

void wowEnvironment::loadCascListFiles()
{
#if defined(MW_USE_CASC)

	CascListFiles.clear();
	
	string_path path = FileSystem->getDataDirectory();
	path.normalizeDir();
	path.append(LISTFILE);
	path.normalize();

	IReadFile* rfile = FileSystem->createAndOpenFile(path.c_str(), false);
	uint32_t size = rfile->getSize();
	if (size > 0)
	{
		uint8_t* buffer = (uint8_t*)Z_AllocateTempMemory(size);
		uint32_t rsize = rfile->read(buffer, size);
		uint8_t* p = buffer;
		uint8_t* end = buffer + rsize;

		while (p <= end)
		{
			uint8_t* q = p;
			do 
			{
				if (*q == '\r' || *q=='\n')
					break;
			} while (q++ <= end);

			string_cs256 filename((const char*)p, (uint32_t)(q-p));
			
			if (filename.length() == 0)
				break;
			filename.normalize();
			filename.make_lower();

			CascListFiles.push_back(filename.c_str());

			p = q;
			while(*p == '\r' || *p == '\n') ++p;
		}	

		Z_FreeTempMemory(buffer);

		std::sort(CascListFiles.begin(), CascListFiles.end());
#if WOW_VER >= 70
		for (int i = 0; i < (int)CascListFiles.size(); ++i)		
		{
			char realfilename[256];
			int filedataId;
			if(2 == sscanf(CascListFiles[i].c_str(), "%s %d", realfilename, &filedataId))	//make map
			{
				FileIdMap[filedataId] = i;
				CascListFiles[i] = realfilename;
			}
		}
#endif
	}
	delete rfile;

	//make root dir index
	DirIndexMap.clear();
	{
		int nFiles = (int)CascListFiles.size();
		for (int i=0; i<nFiles; ++i)
		{
			const char* szFile = CascListFiles[i].c_str();
			const char* p = strchr( szFile, '/');
			while(p)
			{
				string_cs256 dir(szFile, (uint32_t)(p - szFile));
				if (DirIndexMap.find(dir) == DirIndexMap.end())
					DirIndexMap[dir] = i;														//add dir to index
					
				if(*(p+1) == '0')
					break;
				p = strchr(p+1, '/');
			}
		}
	}

#endif
}

bool wowEnvironment::loadRoot()
{
#if defined(MW_USE_MPQ)

	//custom locale
	const char* customlocale = FileSystem->getConfigs()->getSetting(ECT_SETTING, "customlocalempq");
	if (strlen(customlocale))
	{
		string_path path = LocalePath;
		path.append(customlocale);

		char fullpath[QMAX_PATH];
		FileSystem->getAbsolutePath(path.c_str(), fullpath, QMAX_PATH);

		if (FileSystem->isFileExists(fullpath))
		{
			MPQArchive* ar = new MPQArchive(fullpath);
			InsertTailList(&LocaleMpqArchiveList, &ar->Link);

			FileSystem->writeLog(ELOG_RES, "loaded mpq: %s", ar->archivename.c_str());
		}
	}

	//custom base
	const char* custombase = FileSystem->getConfigs()->getSetting(ECT_SETTING, "custombasempq");
	if (strlen(custombase))
	{
		string_path path = FileSystem->getMpqDirectory();
		path.append(custombase);
		char fullpath[QMAX_PATH];
		FileSystem->getAbsolutePath(path.c_str(), fullpath, QMAX_PATH);

		if (FileSystem->isFileExists(fullpath))
		{
			MPQArchive* ar = new MPQArchive(fullpath);
			InsertTailList(&MainMpqArchiveList, &ar->Link);

			FileSystem->writeLog(ELOG_RES, "loaded mpq: %s", ar->archivename.c_str());
		}
	}

	uint32_t patchnum = sizeof(updatefiles) / sizeof(char*);
	//open locale patch
	for (uint32_t k=0; k<patchnum; ++k)
	{
		string_path patch = LocalePath;
		patch.append("wow-update-");
		patch.append(Locale.c_str());
		patch.append("-");
		patch.append(updatefiles[k]);
		patch.append(".mpq");

		char fullpath[QMAX_PATH];
		FileSystem->getAbsolutePath(patch.c_str(), fullpath, QMAX_PATH);

		if (FileSystem->isFileExists(fullpath))
		{
			MPQArchive* ar = new MPQArchive(fullpath);

			InsertTailList(&LocalePatchMpqArchiveList, &ar->Link);
			FileSystem->writeLog(ELOG_RES, "loaded patch mpq: %s", ar->archivename.c_str());
		}
		else
		{
			FileSystem->writeLog(ELOG_RES, "file doesn't exist: %s", fullpath);
		}
	}

	//locale file
	uint32_t localmpqnum = sizeof(localempqfiles) / sizeof(char*);
	for (uint32_t i=0; i<localmpqnum; ++i)
	{
		string_path path = LocalePath;
		path.append(localempqfiles[i]);
		path.append("-");
		path.append(Locale.c_str());
		path.append(".mpq");

		char fullpath[QMAX_PATH];
		FileSystem->getAbsolutePath(path.c_str(), fullpath, QMAX_PATH);

		if (!FileSystem->isFileExists(fullpath))
		{
			char tmp[512];
			Q_sprintf(tmp, 512, "%s doesn't exist, make sure it exists or edit setting.cfg properly!", fullpath);
			
			CSysUtility::messageBoxWarning(tmp);
			CSysUtility::exitProcess(-1);
		}
		
		MPQArchive* ar = new MPQArchive(fullpath);

		//apply patch
		for(PLENTRY e = LocalePatchMpqArchiveList.Flink; e != &LocalePatchMpqArchiveList; )
		{
			MPQArchive* patch = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			bool success = ar->applyPatch(patch);
			ASSERT(success);
		}

		InsertTailList(&LocaleMpqArchiveList, &ar->Link);
		FileSystem->writeLog(ELOG_RES, "loaded mpq: %s", ar->archivename.c_str());
	}

	//open main patch
	for (uint32_t k=0; k<patchnum; ++k)
	{
		string_path patch =  FileSystem->getMpqDirectory();
		patch.append("wow-update-base-");
		patch.append(updatefiles[k]);
		patch.append(".mpq");
		
		char fullpath[QMAX_PATH];
		FileSystem->getAbsolutePath(patch.c_str(), fullpath, QMAX_PATH);

		if (FileSystem->isFileExists(fullpath))
		{
			MPQArchive* ar = new MPQArchive(fullpath);

			InsertTailList(&MainPatchMpqArchiveList, &ar->Link);
			FileSystem->writeLog(ELOG_RES, "loaded patch mpq: %s", ar->archivename.c_str());
		}
		else
		{
			FileSystem->writeLog(ELOG_RES, "file doesn't exist: %s", fullpath);
		}
	}

	//main file
	uint32_t basempqnum = sizeof(basempqfiles) / sizeof(char*);
	for (uint32_t i=0; i<basempqnum; ++i)
	{
		string_path path = FileSystem->getMpqDirectory();
		path.append(basempqfiles[i]);
		char fullpath[QMAX_PATH];
		FileSystem->getAbsolutePath(path.c_str(), fullpath, QMAX_PATH);

		if (!FileSystem->isFileExists(fullpath))
		{
			char tmp[512];
			Q_sprintf(tmp, 512, "%s doesn't exist, make sure it exists or edit setting.cfg properly!", fullpath);

			CSysUtility::messageBoxWarning(tmp);
			CSysUtility::exitProcess(-1);
		}

		MPQArchive* ar = new MPQArchive(fullpath);
		//apply patch
		for(PLENTRY e = MainPatchMpqArchiveList.Flink; e != &MainPatchMpqArchiveList; )
		{
			MPQArchive* patch = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			bool success = ar->applyPatch(patch);
			ASSERT(success);
		}

		InsertTailList(&MainMpqArchiveList, &ar->Link);
		FileSystem->writeLog(ELOG_RES, "loaded mpq: %s", ar->archivename.c_str());
	}

	return true;

#elif defined(MW_USE_CASC)
	if(!CascOpenStorage(FileSystem->getMpqDirectory(), 0, &hStorage))
	{
		char tmp[512];
		Q_sprintf(tmp, 512, "%s is not a valid CASC root, please edit setting.cfg properly!", FileSystem->getMpqDirectory());

		CSysUtility::messageBoxWarning(tmp);
		CSysUtility::exitProcess(-1);
	}

	return true;
#else
	ASSERT(false);
	return false;
#endif
}

void wowEnvironment::unloadRoot()
{
#ifdef MW_USE_MPQ

	for(PLENTRY e = LocaleMpqArchiveList.Flink; e != &LocaleMpqArchiveList; )
	{
		MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
		e = e->Flink;

		ar->close(false);
		delete ar;
	}

	for(PLENTRY e = MainMpqArchiveList.Flink; e != &MainMpqArchiveList; )
	{
		MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
		e = e->Flink;

		ar->close(false);
		delete ar;
	}

	for(PLENTRY e = MainPatchMpqArchiveList.Flink; e != &MainPatchMpqArchiveList; )
	{
		MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
		e = e->Flink;

		ar->close(false);
		delete ar;
	}

	for(PLENTRY e = LocalePatchMpqArchiveList.Flink; e != &LocalePatchMpqArchiveList; )
	{
		MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
		e = e->Flink;

		ar->close(false);
		delete ar;
	}
#elif defined(MW_USE_CASC)
	if(hStorage != nullptr)
		CascCloseStorage(hStorage);
#else
	ASSERT(false);
#endif
}

//读取文件使用临时内存，在打开后需要尽快释放
IMemFile* wowEnvironment::openFile(const char* filename, bool tempfile) const
{
	char realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	unsigned char* buffer = 0;

	string256 strfilename;
	if (UseAlternate)
	{
		strfilename = "alternate/";
		strfilename.append(filename);
		strfilename.normalize();
	}

	if(UseCompress)
	{
#if defined(MW_USE_MPQ)
		for(PLENTRY e = LocaleMpqArchiveList.Flink; e != &LocaleMpqArchiveList; )
		{
			MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			HANDLE& mpq_a = ar->mpq_a;
			HANDLE fh;
			if( !SFileOpenFileEx( mpq_a, realfilename, SFILE_OPEN_FROM_MPQ, &fh ) )
				continue;

			// Found!
			uint32_t size = SFileGetFileSize( fh, nullptr );

			// HACK: in patch.mpq some files don't want to open and give 1 for filesize
			if (size<=1) {
				SFileCloseFile(fh);
				return nullptr;
			}

			//found
			if (tempfile)
				buffer = (unsigned char*)Z_AllocateTempMemory(size);
			else
				buffer = new unsigned char[size];

			bool ret = SFileReadFile(fh, buffer, (DWORD)size, nullptr, nullptr);
			ASSERT(ret);
			SFileCloseFile(fh);

			//record file
			if(RecordFile)
			{
				char tmp[1024];
				Q_sprintf(tmp, 1024, "%s , %d\n", realfilename, 1);
				RecordFile->writeText(tmp, 1024);
				RecordFile->flush();
			}

			return  new CMemFile(buffer, size, realfilename, tempfile);
		}

		for(PLENTRY e = MainMpqArchiveList.Flink; e != &MainMpqArchiveList; )
		{
			MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			HANDLE& mpq_a = ar->mpq_a;
			HANDLE fh;

			if (UseAlternate)
			{
				if( !SFileOpenFileEx( mpq_a, strfilename.c_str(), SFILE_OPEN_FROM_MPQ, &fh ) )
				{
					if( !SFileOpenFileEx( mpq_a, realfilename, SFILE_OPEN_FROM_MPQ, &fh ) )
						continue;
				}
			}
			else
			{
				if( !SFileOpenFileEx( mpq_a, realfilename, SFILE_OPEN_FROM_MPQ, &fh ) )
					continue;
			}
			
			// Found!
			uint32_t size = SFileGetFileSize( fh, nullptr );

			// HACK: in patch.mpq some files don't want to open and give 1 for filesize
			if (size<=1 || size == 0xffffffff) {
				SFileCloseFile(fh);
				return nullptr;
			}

			//found
			if (tempfile)
				buffer = (unsigned char*)Z_AllocateTempMemory(size);
			else
				buffer = new unsigned char[size];

			bool ret = SFileReadFile(fh, buffer, (DWORD)size, nullptr, nullptr);
			ASSERT(ret);
			SFileCloseFile(fh);

			//record file
			if(RecordFile)
			{
				char tmp[1024];
				Q_sprintf(tmp, 1024, "%s , %d\n", realfilename, 0);
				RecordFile->writeText(tmp, 1024);
				RecordFile->flush();
			}

			return new CMemFile(buffer, size, realfilename, tempfile);
		}
#elif defined(MW_USE_CASC)
		string_path path = FileSystem->getDataDirectory();
		path.normalizeDir();
		path.append(MPQFILES);
		path.append(realfilename);
		path.normalize();
		if (FileSystem->isFileExists(path.c_str()))
		{
			IReadFile* rfile = FileSystem->createAndOpenFile(path.c_str(), true);
			uint32_t size = rfile->getSize();

			if (size <= 1 || size == 0xffffffff)
			{
				delete rfile;
				return nullptr;
			}

			if (tempfile)
				buffer = (unsigned char*)Z_AllocateTempMemory(size);
			else
				buffer = new unsigned char[size];

			uint32_t readSize = rfile->read(buffer, size);
			ASSERT(readSize == size);
			delete rfile;

			return new CMemFile(buffer, size, realfilename, tempfile);
		}

		HANDLE hFile;

		if (!CascOpenFile(hStorage, realfilename, CascLocale, 0, &hFile))
		{
			if (CascLocale == 0 || !CascOpenFile(hStorage, realfilename, 0, 0, &hFile))
				return nullptr;
		}

		// Found!
		DWORD dwHigh;
		uint32_t size = CascGetFileSize(hFile, &dwHigh);

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (size<=1 || size == 0xffffffff) {
			CascCloseFile(hFile);
			return nullptr;
		}

		//found
		if (tempfile)
			buffer = (unsigned char*)Z_AllocateTempMemory(size);
		else
			buffer = new unsigned char[size];

		bool ret = CascReadFile(hFile, buffer, (DWORD)size, nullptr);
		if (!ret)
		{
			if (tempfile)
				Z_FreeTempMemory(buffer);
			else
				delete[] buffer;

			CascCloseFile(hFile);
			return nullptr;
		}

		CascCloseFile(hFile);

		//record file
		if(RecordFile)
		{
			char tmp[1024];
			Q_sprintf(tmp, 1024, "%s , %d\n", realfilename, 0);
			RecordFile->writeText(tmp, 1024);
			RecordFile->flush();
		}

		return  new CMemFile(buffer, size, realfilename, tempfile);
#endif
	}
	else
	{
		string_path path = FileSystem->getMpqDirectory();
        path.normalizeDir();
		path.append(MPQFILES);
		path.append(realfilename);
		path.normalize();
		if (!FileSystem->isFileExists(path.c_str()))
		{
			if (!UseLocale)
				return nullptr;

			path = LocalePath;
			path.append(MPQFILES);
			path.append(realfilename);
			path.normalize();
			if (!FileSystem->isFileExists(path.c_str()))
			{
				return nullptr;			//not found
			}
		}

		IReadFile* rfile = FileSystem->createAndOpenFile(path.c_str(), true);
		uint32_t size = rfile->getSize();

		if (size <= 1)
		{
			delete rfile;
			return nullptr;
		}

		if (tempfile)
			buffer = (unsigned char*)Z_AllocateTempMemory(size);
		else
			buffer = new unsigned char[size];

		uint32_t readSize = rfile->read(buffer, size);
		ASSERT(readSize == size);
		delete rfile;

		return new CMemFile(buffer, size, realfilename, tempfile);
	}

	return nullptr;
}

bool wowEnvironment::exists(const char* filename) const
{
	if (strlen(filename) == 0)
		return false;

	char realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);
	
	if(UseCompress)
	{
#if defined(MW_USE_MPQ)
		for(PLENTRY e = LocaleMpqArchiveList.Flink; e != &LocaleMpqArchiveList; )
		{
			MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			HANDLE& mpq_a = ar->mpq_a;
			if( SFileHasFile( mpq_a, filename) )
				return true;
		}
		for(PLENTRY e = MainMpqArchiveList.Flink; e != &MainMpqArchiveList; )
		{
			MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			HANDLE& mpq_a = ar->mpq_a;
			if( SFileHasFile( mpq_a, realfilename) )
				return true;
		}
#elif defined(MW_USE_CASC)
		HANDLE hFile;
		if (!CascOpenFile(hStorage, realfilename, CascLocale, 0, &hFile))
			return false;

		CascCloseFile(hFile);
		return true;
#endif
	}
	else
	{
		string_path path = FileSystem->getMpqDirectory();
		path.append(MPQFILES);
		path.append(realfilename);

#ifdef MW_COMPILE_WITH_GLES2
#if defined(USE_PVR)
		path.changeExt(".blp", ".pvr");
#elif defined(USE_KTX)
		path.changeExt(".blp", ".ktx");
#endif
#endif

		path.normalize();

		if (!FileSystem->isFileExists(path.c_str()))
		{
			if (!UseLocale)
				return false;

			path = LocalePath;
			path.append(MPQFILES);
			path.append(filename);

#ifdef MW_COMPILE_WITH_GLES2
#if defined(USE_PVR)
			path.changeExt(".blp", ".pvr");
#elif defined(USE_KTX)
			path.changeExt(".blp", ".ktx");
#endif
#endif

			path.normalize();
			if (!FileSystem->isFileExists(path.c_str()))
				return false;			//not found
		}
		return true;
	}

	return false;
}

void wowEnvironment::iterateFiles(const char* ext, MPQFILECALLBACK callback, void* param) const
{
	if(UseCompress)
	{
#if defined(MW_USE_MPQ)
		for(PLENTRY e = LocaleMpqArchiveList.Flink; e != &LocaleMpqArchiveList; )
		{
			MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			HANDLE& mpq_a = ar->mpq_a;
			HANDLE fh;
			if( !SFileOpenFileEx( mpq_a, "(listfile)", SFILE_OPEN_ANY_LOCALE, &fh ) )
				continue;

			size_t size = SFileGetFileSize( fh, nullptr );

			if (size > 0)
			{
				uint8_t* buffer = (uint8_t*)Z_AllocateTempMemory(size);
				SFileReadFile(fh, buffer, (DWORD)size, nullptr, nullptr);
				uint8_t* p = buffer;
				uint8_t* end = buffer + size;

				while (p <= end)
				{
					uint8_t* q = p;
					do 
					{
						if (*q == '\r')
							break;
					} while (q++ <= end);

					string256 filename((const char*)p, (uint32_t)(q-p));
					if (filename.length() == 0)
						break;
					filename.normalize();

					p = q + 2;			//\r\n

					if (hasFileExtensionA(filename.c_str(), ext))
					{
						callback(filename.c_str(), param);
					}
				}	

				Z_FreeTempMemory(buffer);
			}
			SFileCloseFile( fh );
		}

		for(PLENTRY e = MainMpqArchiveList.Flink; e != &MainMpqArchiveList; )
		{
			MPQArchive* ar = reinterpret_cast<MPQArchive*>CONTAINING_RECORD(e, MPQArchive, Link);
			e = e->Flink;

			HANDLE& mpq_a = ar->mpq_a;
			HANDLE fh;
			if( !SFileOpenFileEx( mpq_a, "(listfile)", SFILE_OPEN_ANY_LOCALE, &fh ) )
				continue;

			size_t size = SFileGetFileSize( fh, nullptr );

			if (size > 0)
			{
				uint8_t* buffer = (uint8_t*)Z_AllocateTempMemory(size);
				SFileReadFile(fh, buffer, (DWORD)size, nullptr, nullptr);
				uint8_t* p = buffer;
				uint8_t* end = buffer + size;

				while (p <= end)
				{
					uint8_t* q = p;
					do 
					{
						if (*q == '\r' || *q=='\n')
							break;
					} while (q++ <= end);

					string256 filename((const char*)p, (uint32_t)(q-p));
					if (filename.length() == 0)
						break;
					filename.normalize();

					p = q;
					while(*p == '\r' || *p == '\n') ++p;

					if (hasFileExtensionA(filename.c_str(), ext))
					{
						callback(filename.c_str(), param);
					}
				}	

				Z_FreeTempMemory(buffer);
			}
			SFileCloseFile( fh );
		}

#elif defined(MW_USE_CASC)
		{
			uint32_t count = (uint32_t)CascListFiles.size();
			for (uint32_t i=0; i<count; ++i)
			{
				const char* filename = CascListFiles[i].c_str();
				if (hasFileExtensionA(filename, ext))
				{
					callback(filename, param);
				}
			}
		}
#endif
	}
	else
	{
		string_path path = FileSystem->getMpqDirectory();
		path.append(MPQFILES);
		path.normalize();

		Q_iterateFiles(path.c_str(), ext, callback, param, FileSystem->getMpqDirectory());

		if(UseLocale)
		{
			path = LocalePath;
			path.append(MPQFILES);
			Q_iterateFiles(path.c_str(), ext, callback, param, FileSystem->getMpqDirectory());
		}
	}

}

void wowEnvironment::iterateFiles(const char* path, const char* ext, MPQFILECALLBACK callback, void* param) const
{
#if defined(MW_USE_CASC)
	{
		string_cs256 strBaseDir(path);
		strBaseDir.make_lower();
		auto itr = DirIndexMap.find(strBaseDir);
		if (itr == DirIndexMap.end())
			return;

		//calc start
		int nStart = itr->second;

		uint32_t count = (uint32_t)CascListFiles.size();
		for (uint32_t i=nStart; i<count; ++i)
		{
			const char* filename = CascListFiles[i].c_str();
			if (hasFileExtensionA(filename, ext))
			{
				callback(filename, param);
			}

			if (strstr(filename, strBaseDir.c_str()) == 0)
				break;
		}
	}
#endif
}

void wowEnvironment::clearOwnCascFiles()
{
	OwnCascSet.clear();
}

void wowEnvironment::addOwnCascFile(const char* filename)
{
	string_cs256 fname(filename);
	fname.normalize();
	fname.make_lower();
	OwnCascSet.insert(fname);
}

void wowEnvironment::finishOwnCascFiles()
{
	OwnCascFiles.clear();
	for (auto itr = OwnCascSet.begin(); itr != OwnCascSet.end(); ++itr)
	{
		OwnCascFiles.push_back(*itr);	
	}
	OwnCascSet.clear();

	//make root dir index
	OwnDirIndexMap.clear();
	{
		int nFiles = (int)OwnCascFiles.size();
		for (int i=0; i<nFiles; ++i)
		{
			const char* szFile = OwnCascFiles[i].c_str();
			const char* p = strchr( szFile, '/');
			while(p)
			{
				string_cs256 dir(szFile, (uint32_t)(p - szFile));
				if (OwnDirIndexMap.find(dir) == OwnDirIndexMap.end())
					OwnDirIndexMap[dir] = i;														//add dir to index

				if(*(p+1) == '0')
					break;
				p = strchr(p+1, '/');
			}
		}
	}
}

void wowEnvironment::getCascLocale()
{
#ifdef MW_USE_CASC
	if (Locale == "enUS")
		CascLocale = CASC_LOCALE_ENUS;
	else if (Locale == "koKR")
		CascLocale = CASC_LOCALE_KOKR;
	else if (Locale == "frFR")
		CascLocale = CASC_LOCALE_FRFR;
	else if (Locale == "deDE")
		CascLocale = CASC_LOCALE_DEDE;
	else if (Locale == "zhCN")
		CascLocale = CASC_LOCALE_ZHCN;
	else if (Locale == "esES")
		CascLocale = CASC_LOCALE_ESES;
	else if (Locale == "ZhTW")
		CascLocale = CASC_LOCALE_ZHTW;
	else if (Locale == "enGB")
		CascLocale = CASC_LOCALE_ENGB;
	else if (Locale == "enCN")
		CascLocale = CASC_LOCALE_ENCN;
	else if (Locale == "enTW")
		CascLocale = CASC_LOCALE_ENTW;
	else if (Locale == "esMX")
		CascLocale = CASC_LOCALE_ESMX;
	else if (Locale == "ruRU")
		CascLocale = CASC_LOCALE_RURU;
	else if (Locale == "ptBR")
		CascLocale = CASC_LOCALE_PTBR;
	else if (Locale == "itIT")
		CascLocale = CASC_LOCALE_ITIT;
	else if (Locale == "ptPT")
		CascLocale = CASC_LOCALE_PTPT;
	else
		CascLocale = 0;
#endif
}

void wowEnvironment::getFiles(const char* baseDir, const char* ext, std::vector<string_cs256>& files, bool useOwn)
{
	string_cs256 dir(baseDir);
	dir.normalizeDir();
	dir.make_lower();

	files.clear();
	const std::vector<string_cs256>&	listFiles = useOwn ? OwnCascFiles : CascListFiles;
	T_DirIndexMap& dirIndexMap = useOwn ? OwnDirIndexMap : DirIndexMap;

	if (dir.empty())		//root
	{
		return;	
		/*
		int nFiles = (int)CascListFiles.size();
		for (int i=0; i<nFiles; ++i)
		{
			const char* szFile = CascListFiles[i].c_str();
			const char* p = strchr( szFile, '/');
			if(!p && hasFileExtensionA(szFile, ext))
				files.push_back(szFile);
		}
		*/
	}
	else
	{
		string_cs256 strBaseDir(baseDir);
		strBaseDir.make_lower();
		T_DirIndexMap::iterator itr = dirIndexMap.find(strBaseDir);
		if (itr == dirIndexMap.end())
			return;

		//calc start
		int nStart = itr->second;

		bool bFind = false;
		int nFiles = (int)listFiles.size();
		for (int i=nStart; i<nFiles; ++i)
		{
			const char* szFile = listFiles[i].c_str();
			const char* p = strstr(szFile, dir.c_str());
			if (p)
			{
				bFind = true;

				const char* p1 = p + dir.length();
				const char* ret = strchr(p1, '/');		//包含目录，但不能再有'/'字符
				if (!ret && hasFileExtensionA(p1, ext))
					files.push_back(szFile);
			}

			if (bFind && !p)			//已经能找到第一个子目录，由于CascListFiles排序，下一个不包含的直接退出循环
				break;
		}
	}
}

void wowEnvironment::getDirectories(const char* baseDir, std::vector<string_cs256>& outdirs, bool useOwn)
{
	string512 dir(baseDir);
	dir.normalizeDir();
	dir.make_lower();

	outdirs.clear();
	const std::vector<string_cs256>&	listFiles = useOwn ? OwnCascFiles : CascListFiles;
	T_DirIndexMap& dirIndexMap = useOwn ? OwnDirIndexMap : DirIndexMap;

	std::set<string_cs256, std::less<string_cs256>, qzone_allocator<string_cs256> > dirSet;

	if (dir.empty())		//root
	{
		for (T_DirIndexMap::iterator itr = dirIndexMap.begin(); itr != dirIndexMap.end(); ++itr)
		{
			int idx = itr->second;
			const char* szFile = listFiles[idx].c_str();
			const char* p = strchr( szFile, '/');
			if (p)
			{
				string_cs256 dir(szFile, (uint32_t)(p - szFile));
				dirSet.insert(dir);
			}
		}
	}
	else
	{
		string_cs256 strBaseDir(baseDir);
		strBaseDir.make_lower();
		T_DirIndexMap::iterator itr = dirIndexMap.find(strBaseDir);
		if (itr == dirIndexMap.end())
			return;

		//calc start
		int nStart = itr->second;

		bool bFind = false;
		int nFiles = (int)listFiles.size();
		for (int i=nStart; i<nFiles; ++i)
		{
			const char* szFile = listFiles[i].c_str();
			const char* p = strstr(szFile, dir.c_str());
			if (p)
			{
				bFind = true;

				const char* p1 = p + dir.length();
				const char* p2 = strchr(p1, '/');		//包含目录，需要有'/'字符
				if (p2)
				{
					string_cs256 dir(szFile, (uint32_t)(p2 - szFile));
					dirSet.insert(dir);
				}
			}

			if (bFind && !p)			//已经能找到第一个子目录，由于CascListFiles排序，下一个不包含的直接退出循环
				break;
		}
	}

	for (auto itr = dirSet.begin(); itr != dirSet.end(); ++itr)
	{
		outdirs.push_back(*itr);
	}
}

const char* wowEnvironment::getFileNameByFileDataId(int filedataId)
{
	auto itr = FileIdMap.find(filedataId);
	if (itr == FileIdMap.end())
		return "";
	int index = itr->second;
	return CascListFiles[index].c_str();
}
