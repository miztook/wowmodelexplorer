#include "stdafx.h"
#include "editor_ResourceLoader.h"

IFileWDT* ResourceLoader_loadWDT( const c8* mapname, s32 id, bool simple )
{
	c8 fullmapname[MAX_PATH];
	sprintf_s(fullmapname, MAX_PATH, "World\\Maps\\%s\\%s.wdt", mapname, mapname);
	return g_Engine->getResourceLoader()->loadWDT(fullmapname, id, simple);
}

void ResourceLoader_unloadWDT( IFileWDT* wdt )
{
	if (wdt)
		wdt->drop();
}

