#pragma once

#include "core.h"

#ifdef MW_USE_MPQ

class MPQArchive
{
public:
	explicit MPQArchive(const char* filename);
	~MPQArchive();
public:
	bool openPatch(const char* patchname);
	bool applyPatch(MPQArchive* patchArchive);
	void close(bool freepatch);

public:
	LENTRY	Link;

	HANDLE	mpq_a;
	string256	archivename;
};

#endif