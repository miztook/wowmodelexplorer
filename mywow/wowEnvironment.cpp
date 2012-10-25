#include "stdafx.h"
#include "wowEnvironment.h"
#include "mpq_stormlib.h"
#include "mywow.h"

const c8* baseupdatefiles[] =
{
	"wow-update-base-16016.mpq",
	"wow-update-base-16048.mpq",
	"wow-update-base-16057.mpq",
};

const c8* basempqfiles[] = 
{
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

const c8* localempqfiles[] =
{
	"locale",
// 	"expansion1-locale",
// 	"expansion2-locale",
// 	"expansion3-locale"
};

const c8* localeupdatefiles[] =
{
	"14769",
	"14656",
};

wowEnvironment::wowEnvironment(IFileSystem* fs, E_LANG_ID langid)
	: LangId(langid)
{
	const c8* langString = getLangString(langid);
	strcpy_s(LocalePath, MAX_PATH, fs->getBaseDirectory());
	strcat_s(LocalePath, MAX_PATH, langString);
	strcat_s(LocalePath, "\\");

	loadWowArchives(fs, langid);
}

wowEnvironment::~wowEnvironment()
{
	unloadWowArchives();
}

bool wowEnvironment::loadWowArchives(IFileSystem* fs, E_LANG_ID langid)
{
	const c8* langString = getLangString(langid);

	//main file
	u32 basempqnum = sizeof(basempqfiles) / sizeof(c8*);
	for (u32 i=0; i<basempqnum; ++i)
	{
		string256 path = fs->getBaseDirectory();
		path.append(basempqfiles[i]);

		if (_access_s(path.c_str(), 0) != 0)
		{
			c8 tmp[512];
			sprintf_s(tmp, 512, "%s doesn't exist，make sure it's existed!", path.c_str());
			MessageBox(NULL, tmp, "", MB_ICONEXCLAMATION);
			::ExitProcess(-1);
		}

		MPQArchive* ar = new MPQArchive(path.c_str());
		ar->isLocale = false;
		//apply patch
		ar->applyPatch(baseupdatefiles, sizeof(baseupdatefiles) / sizeof(c8*));

		MpqArchives.push_back(ar);
	}
	
	//update file
// 	for (u32 i=0; i<2; ++i)
// 	{
// 		string256 path = LocalePath;
// 		path.append("wow-update-");
// 		path.append(langString);
// 		path.append("-");
// 		path.append(localeupdatefiles[i]);
// 		path.append(".mpq");
// 
// 		MPQArchive* ar = new MPQArchive(path.c_str());
// 		MpqArchives.push_back(ar);
// 	}



	//main file
	u32 localmpqnum = sizeof(localempqfiles) / sizeof(c8*);
	for (u32 i=0; i<localmpqnum; ++i)
	{
		string256 path = LocalePath;
		path.append(localempqfiles[i]);
		path.append("-");
		path.append(langString);
		path.append(".mpq");

		if (_access_s(path.c_str(), 0) != 0)
		{
			c8 tmp[512];
			sprintf_s(tmp, 512, "%s doesn't exist，make sure it's existed!", path.c_str());
			MessageBox(NULL, tmp, "", MB_ICONEXCLAMATION);
			::ExitProcess(-1);
		}

		MPQArchive* ar = new MPQArchive(path.c_str());
		ar->isLocale = true;
		
		MpqArchives.push_back(ar);
	}

	return true;
}

void wowEnvironment::unloadWowArchives()
{
	for (T_MpqArchives::iterator itr=MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		delete (*itr);
	}
}

//读取文件使用临时内存，在打开后需要尽快释放
MPQFile* wowEnvironment::openFile( const c8* filename, bool tempfile )
{
	int size = 0;
	unsigned char* buffer = 0;

	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		HANDLE& mpq_a = (*itr)->mpq_a;
		HANDLE fh;
		if( !SFileOpenFileEx( mpq_a, filename, SFILE_OPEN_FROM_MPQ, &fh ) )
			continue;

		// Found!
		size = SFileGetFileSize( fh, NULL );

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (size<=1) {
			return NULL;
		}
		if (tempfile)
			buffer = (unsigned char*)Hunk_AllocateTempMemory(size);
		else
			buffer = new unsigned char[size];

		SFileReadFile(fh, buffer, (DWORD)size, NULL, NULL);
		SFileCloseFile(fh);
		return new MPQFile(buffer, size, filename, tempfile);
	}

	return NULL;
}

bool wowEnvironment::exists( const c8* filename )
{
	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		HANDLE& mpq_a = (*itr)->mpq_a;
		if( SFileHasFile( mpq_a, filename) )
			return true;
	}

	return false;
}

s32 wowEnvironment::getSize( const c8* filename )
{
	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		HANDLE& mpq_a = (*itr)->mpq_a;
		HANDLE fh;
		if( !SFileOpenFileEx( mpq_a, filename, SFILE_OPEN_FROM_MPQ, &fh ) )
			continue;

		DWORD filesize = SFileGetFileSize( fh, NULL );
		SFileCloseFile( fh );
		return (s32)filesize;
	}

	return 0;
}

void wowEnvironment::iterateFiles(const c8* ext, MPQFILECALLBACK callback, void* param )
{
	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		if (!(*itr)->isLocale)
			continue;

		HANDLE& mpq_a = (*itr)->mpq_a;
		HANDLE fh;
		if( !SFileOpenFileEx( mpq_a, "(listfile)", SFILE_OPEN_FROM_MPQ, &fh ) )
			continue;

		size_t size = SFileGetFileSize( fh, NULL );

		if (size > 0)
		{
			u8* buffer = (u8*)Hunk_AllocateTempMemory(size);
			SFileReadFile(fh, buffer, (DWORD)size, NULL, NULL);
			u8* p = buffer;
			u8* end = buffer + size;

			while (p <= end)
			{
				u8* q = p;
				do 
				{
					if (*q == '\r')
						break;
				} while (q++ <= end);

				string256 filename((const c8*)p, q-p);
				if (filename.length() == 0)
					break;

				p = q + 2;			//\r\n

				if (hasFileExtensionA(filename.c_str(), ext))
				{
					callback(filename.c_str(), param);
				}
			}	

			Hunk_FreeTempMemory(buffer);
		}
		SFileCloseFile( fh );
	}
}


