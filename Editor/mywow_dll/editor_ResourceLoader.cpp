#include "stdafx.h"
#include "editor_ResourceLoader.h"

IFileWDT* ResourceLoader_loadWDT( const char* mapname, int32_t id, bool simple )
{
	char fullmapname[MAX_PATH];
	sprintf_s(fullmapname, MAX_PATH, "World\\Maps\\%s\\%s.wdt", mapname, mapname);
	return g_Engine->getResourceLoader()->loadWDT(fullmapname, id, simple);
}

void ResourceLoader_unloadWDT( IFileWDT* wdt )
{
	if (wdt)
		wdt->drop();
}

