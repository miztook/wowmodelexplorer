#include "stdafx.h"
#include "mpq_stormlib.h"

#ifdef MW_USE_MPQ

#include "StormLib.h"

MPQArchive::MPQArchive( const c8* filename )
	: archivename(filename)
{
	bool result = SFileOpenArchive(filename, 0, MPQ_OPEN_FORCE_MPQ_V1|MPQ_OPEN_READ_ONLY, &mpq_a );

	ASSERT(result);

	InitializeListHead(&Link);
}

MPQArchive::~MPQArchive()
{
	
}

bool MPQArchive::openPatch( const c8* patchname )
{
	c8 dir[QMAX_PATH];
	getFileDirA(archivename.c_str(), dir, QMAX_PATH);

	string256 path = dir;
	path.append("\\");
	path.append(patchname);
	return SFileOpenPatchArchive(mpq_a, path.c_str(), "", 0);
}

bool MPQArchive::applyPatch( MPQArchive* patchArchive )
{
	return SFileAddPatchArchive(mpq_a, patchArchive->mpq_a, "", 0);
}

void MPQArchive::close(bool freepatch)
{
	SFileCloseArchive(mpq_a, freepatch);
}

#endif

