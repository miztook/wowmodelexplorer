#pragma once

#include "q_memory.h"
#include <cstdlib>
#include "CSysGlobal.h"
#include "CSysSync.h"

void* Z_AllocateTempMemory( size_t size );
void Z_FreeTempMemory( void *buf );

inline void* Z_AllocateTempMemory( size_t size )
{
#ifdef MW_PLATFORM_WINDOWS
	void* p = nullptr;

	BEGIN_LOCK(&g_Globals.tempCS);
	if (Z_AvailableTempMemory() > size)
		p = T_TagMalloc(size, TAG_TEMP);
	END_LOCK(&g_Globals.tempCS);

	if (p)
		return p;
#endif
	return malloc(size);
}

inline void Z_FreeTempMemory( void *buf )
{
#ifdef MW_PLATFORM_WINDOWS
	bool ret;
	BEGIN_LOCK(&g_Globals.tempCS);
	ret = T_Free(buf);
	END_LOCK(&g_Globals.tempCS);

	if (!ret)
#endif
		free(buf);
}