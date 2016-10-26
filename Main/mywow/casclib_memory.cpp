#include "stdafx.h"
#include "mywow.h"

#ifdef MW_USE_CASC

#include "CascLib.h"

#define  CASCLIB_MEMORY

#ifdef CASCLIB_MEMORY

void* Malloc(size_t nSize)
{
	void* p = NULL_PTR;
	if (nSize <= 4)
	{
		BEGIN_LOCK(&g_Globals.allocatorCS);
		if (Z_AvailableSmallMemory() > 4)
			p = Z_TagMalloc(nSize, TAG_SMALL);
		END_LOCK(&g_Globals.allocatorCS);
	}
	else if (nSize <= 64)
	{
		BEGIN_LOCK(&g_Globals.allocatorCS);
		if (Z_AvailableMainMemory() > 64)
			p = Z_TagMalloc(nSize, TAG_GENERAL);
		END_LOCK(&g_Globals.allocatorCS);
	}

	if (!p)
		p = malloc(nSize);

	return p;
}

void Free(void * ptr)
{
	if (!ptr)
		return;

	bool ret;
	BEGIN_LOCK(&g_Globals.allocatorCS);
	ret = Z_Free(ptr);
	END_LOCK(&g_Globals.allocatorCS);

	if (!ret)
		free(ptr);
}

void* ReAlloc(void* ptr, size_t nSize)
{
	bool ret = false;

	void* p = NULL_PTR;
	if (nSize <= 4)
	{
		BEGIN_LOCK(&g_Globals.allocatorCS);
		if (Z_AvailableSmallMemory() > 4)
			p = Z_Realloc(ptr, nSize, TAG_SMALL, ret);
		END_LOCK(&g_Globals.allocatorCS);
	}
	else if (nSize <= 64)
	{
		BEGIN_LOCK(&g_Globals.allocatorCS);
		if (Z_AvailableMainMemory() > 64)
			p = Z_Realloc(ptr, nSize, TAG_GENERAL, ret);
		END_LOCK(&g_Globals.allocatorCS);
	}

	if (ret)
		return p;

	return realloc(ptr, nSize);
}

void* TempMalloc(size_t nSize)
{
	return Z_AllocateTempMemory(nSize);
}

void TempFree(void* ptr)
{
	Z_FreeTempMemory(ptr);
}

#else


void* Malloc(size_t nSize)
{
	return malloc(nSize);
}

void Free(void * ptr)
{
	free(ptr);
}

void* TempMalloc(size_t nSize)
{
	return malloc(nSize);
}

void TempFree(void* ptr)
{
	free(ptr);
}

#endif

#endif
