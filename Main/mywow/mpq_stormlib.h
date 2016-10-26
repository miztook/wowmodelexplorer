#pragma once

#include "core.h"

#ifdef MW_USE_MPQ

class MPQArchive
{
public:
	explicit MPQArchive(const c8* filename);
	~MPQArchive();
public:
	bool openPatch(const c8* patchname);
	bool applyPatch(MPQArchive* patchArchive);
	void close(bool freepatch);

public:
	LENTRY	Link;

	HANDLE	mpq_a;
	string256	archivename;
};

#endif