#include "stdafx.h"
#include "wowEnvironment.h"
#include "mpq_libmpq.h"
#include "mywow.h"

const c8* baseupdatefiles[] =
{
	"wow-update-base-15211.mpq",
	"wow-update-base-15079.mpq",
	"wow-update-base-15005.mpq",
	"wow-update-base-14769.mpq",
	"wow-update-base-14656.mpq",
};

const c8* basempqfiles[] = 
{
	"art.mpq",
	"expansion1.mpq",
	"expansion2.mpq",
	"expansion3.mpq",
//	"world.mpq"
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

	//upate file
// 	for (u32 i=0; i<5; ++i)
// 	{
// 		string256 path = fs->getBaseDirectory();
// 		path.append(baseupdatefiles[i]);
// 
// 		MPQArchive* ar = new MPQArchive(path.c_str());
// 		MpqArchives.push_back(ar);
// 	}

	//main file
	for (u32 i=0; i<4; ++i)
	{
		string256 path = fs->getBaseDirectory();
		path.append(basempqfiles[i]);

		if (_access_s(path.c_str(), 0) != 0)
		{
			c8 tmp[512];
			sprintf_s(tmp, 512, "%s doesn't exist，make sure it exists!", path.c_str());
			MessageBox(NULL, tmp, "", MB_ICONEXCLAMATION);
			::ExitProcess(-1);
		}

		MPQArchive* ar = new MPQArchive(path.c_str());
		ar->isLocale = false;
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
	for (u32 i=0; i<1; ++i)
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
MPQFile* wowEnvironment::openFile( const c8* filename )
{
	int size = 0;
	unsigned char* buffer = 0;

	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		mpq_archive& mpq_a = (*itr)->mpq_ar;
		int fileno = libmpq_file_number(&mpq_a, filename);
		if (fileno == LIBMPQ_EFILE_NOT_FOUND)
			continue;

		// Found!
		size = libmpq_file_info(&mpq_a, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (size<=1) {
			return NULL;
		}

		//buffer = new unsigned char[size];
		buffer = (unsigned char*)Hunk_AllocateTempMemory(size);
		libmpq_file_getdata(&mpq_a, fileno, buffer);
		return new MPQFile(buffer, size, filename);
	}

	return NULL;
}

bool wowEnvironment::exists( const c8* filename )
{
	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		mpq_archive& mpq_a = (*itr)->mpq_ar;
		int fileno = libmpq_file_number(&mpq_a, filename);
		if (fileno != LIBMPQ_EFILE_NOT_FOUND) 
			return true;
	}

	return false;
}

s32 wowEnvironment::getSize( const c8* filename )
{
	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		mpq_archive& mpq_a = (*itr)->mpq_ar;
		int fileno = libmpq_file_number(&mpq_a, filename);
		if (fileno != LIBMPQ_EFILE_NOT_FOUND)
			return libmpq_file_info(&mpq_a, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);
	}

	return 0;
}

void wowEnvironment::iterateFiles(const c8* ext, MPQFILECALLBACK callback, void* param )
{
	for (T_MpqArchives::iterator itr = MpqArchives.begin(); itr != MpqArchives.end(); ++itr)
	{
		if (!(*itr)->isLocale)
			continue;

		mpq_archive& mpq_a = (*itr)->mpq_ar;
		int fileno = libmpq_file_number(&mpq_a, "(listfile)");

		if (fileno != LIBMPQ_EFILE_NOT_FOUND)
		{
			size_t size = libmpq_file_info(&mpq_a, LIBMPQ_FILE_UNCOMPRESSED_SIZE, fileno);

			if (size > 0)
			{
				u8* buffer = (u8*)Hunk_AllocateTempMemory(size);
				libmpq_file_getdata(&mpq_a, fileno, buffer);
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
		}
	}
}


