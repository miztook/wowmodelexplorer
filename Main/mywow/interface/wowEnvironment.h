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
	IMemFile* openFile(const c8* filename, bool tempfile = true);

	bool exists(const c8* filename);

	const c8* getLocale() const { return Locale.c_str(); }
	const c8* getLocalePath() const { return LocalePath; }
	IFileSystem* getFileSystem() const { return FileSystem; }
	u32 getCascFileCount() const { return (u32)CascListFiles.size(); }
	const c8* getCascFile(int index) const { return CascListFiles[index].c_str(); }

	typedef void (*MPQFILECALLBACK)(const c8* filename, void* param);

	void iterateFiles(const c8* ext, MPQFILECALLBACK callback, void* param);
	void iterateFiles(const c8* path, const c8* ext, MPQFILECALLBACK callback, void* param);

	//
	void clearOwnCascFiles();
	void addOwnCascFile(const c8* filename);
	void finishOwnCascFiles();

	void getFiles(const c8* baseDir, const c8* ext, std::vector<string_cs256>& files, bool useOwn);
	void getDirectories(const c8* baseDir, std::vector<string_cs256>& dirs, bool useOwn);

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

	std::vector<string_cs256>		CascListFiles;
	T_DirIndexMap		DirIndexMap;
	std::set<string_cs256, std::less<string_cs256>, qzone_allocator<string_cs256> > OwnCascSet;				//ÓÐÐò
	std::vector<string_cs256>		OwnCascFiles;
	T_DirIndexMap		OwnDirIndexMap;

	string32		Locale;
	c8		LocalePath[QMAX_PATH];
	u32		CascLocale;
	HANDLE	hStorage;

	bool		UseAlternate;
	bool		UseLocale;
	bool		UseCompress;
};
