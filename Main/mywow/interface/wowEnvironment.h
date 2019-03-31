#pragma once

#include "base.h"
#include "core.h"
#include <vector>
#include <set>
#include <unordered_map>

class IFileSystem;
class MPQArchive;
class IMemFile;
class IWriteFile;

class wowEnvironment
{
public:
	wowEnvironment(IFileSystem* fs, bool useCompress, bool outputFilename);
	~wowEnvironment();

public:
	IMemFile* openFile(const char* filename, bool tempfile = true) const;

	bool exists(const char* filename) const;

	const char* getLocale() const { return Locale.c_str(); }
	const char* getLocalePath() const { return LocalePath; }
	IFileSystem* getFileSystem() const { return FileSystem; }
	uint32_t getCascFileCount() const { return (uint32_t)CascListFiles.size(); }
	const char* getCascFile(int index) const { return CascListFiles[index].c_str(); }

	typedef void (*MPQFILECALLBACK)(const char* filename, void* param);

	void iterateFiles(const char* ext, MPQFILECALLBACK callback, void* param) const;
	void iterateFiles(const char* path, const char* ext, MPQFILECALLBACK callback, void* param) const;

	//
	void clearOwnCascFiles();
	void addOwnCascFile(const char* filename);
	void finishOwnCascFiles();

	void getFiles(const char* baseDir, const char* ext, std::vector<string_cs256>& files, bool useOwn);
	void getDirectories(const char* baseDir, std::vector<string_cs256>& dirs, bool useOwn);
	const char* getFileNameByFileDataId(int filedataId);

private:
	bool loadRoot();
	void unloadRoot();

	void loadCascListFiles();

	void getCascLocale();

private:	
#ifdef USE_QALLOCATOR
	typedef std::map<string_cs256, int, std::less<string_cs256>, qzone_allocator<std::pair<string_cs256, int>>> T_DirIndexMap;
#else
	typedef std::unordered_map<string_cs256, int, string_cs256::string_cs_hash> T_DirIndexMap;
#endif

	LENTRY	LocaleMpqArchiveList;
	LENTRY	MainMpqArchiveList;
	LENTRY	LocalePatchMpqArchiveList;
	LENTRY	MainPatchMpqArchiveList;

	IFileSystem*		FileSystem;
	IWriteFile*		RecordFile;

	std::map<int, int, std::less<int>, qzone_allocator<std::pair<int, int> > >	FileIdMap;		
	std::vector<string_cs256>		CascListFiles;
	T_DirIndexMap		DirIndexMap;
	std::set<string_cs256, std::less<string_cs256>, qzone_allocator<string_cs256> > OwnCascSet;				//ÓÐÐò
	std::vector<string_cs256>		OwnCascFiles;
	T_DirIndexMap		OwnDirIndexMap;

	string32		Locale;
	char		LocalePath[QMAX_PATH];
	uint32_t		CascLocale;
	HANDLE	hStorage;

	bool		UseAlternate;
	bool		UseLocale;
	bool		UseCompress;
};
