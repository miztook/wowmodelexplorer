#pragma once

#include "base.h"
#include "core.h"
#include <list>

class IFileSystem;
class MPQArchive;
class MPQFile;

class wowEnvironment
{
public:
	wowEnvironment(IFileSystem* fs, E_LANG_ID langid);
	~wowEnvironment();

public:
	MPQFile* openFile(const c8* filename);

	bool exists(const c8* filename);
	s32 getSize(const c8* filename);

	const c8* getLocalePath() const { return LocalePath; }
	E_LANG_ID getLanguageID() const { return LangId; }

	typedef void (*MPQFILECALLBACK)(const c8* filename, void* param);

	void iterateFiles(const c8* ext, MPQFILECALLBACK callback, void* param);

private:
	bool loadWowArchives(IFileSystem* fs, E_LANG_ID langid);
	void unloadWowArchives();

private:
	typedef std::list<MPQArchive*, qzone_allocator<MPQArchive*>> T_MpqArchives;
	T_MpqArchives MpqArchives;

	E_LANG_ID		LangId;

	c8		LocalePath[MAX_PATH];
};
