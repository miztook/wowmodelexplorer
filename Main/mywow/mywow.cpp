#include "StdAfx.h"
#include "mywow.h"
#include "CSysGlobal.h"

SGlobal g_Globals;

void initGlobal()
{
	INIT_LOCK(&g_Globals.refCS);
	INIT_LOCK(&g_Globals.allocatorCS);
	INIT_LOCK(&g_Globals.tempCS);

// 	INIT_LOCK(&g_Globals.textureCS);
// 	INIT_LOCK(&g_Globals.hwbufferCS);
// 	INIT_LOCK(&g_Globals.vaoCS);
// 	INIT_LOCK(&g_Globals.adtCS);
// 	INIT_LOCK(&g_Globals.m2CS);
// 	INIT_LOCK(&g_Globals.wmoCS);
	INIT_LOCK(&g_Globals.logCS);
}

void deleteGlobal()
{
	DESTROY_LOCK(&g_Globals.logCS);
// 	DESTROY_LOCK(&g_Globals.wmoCS);
// 	DESTROY_LOCK(&g_Globals.m2CS);
// 	DESTROY_LOCK(&g_Globals.adtCS);
// 	DESTROY_LOCK(&g_Globals.vaoCS);
// 	DESTROY_LOCK(&g_Globals.hwbufferCS);
// 	DESTROY_LOCK(&g_Globals.textureCS);

	DESTROY_LOCK(&g_Globals.tempCS);
	DESTROY_LOCK(&g_Globals.allocatorCS);
	DESTROY_LOCK(&g_Globals.refCS);
}

void createEngine(const SEngineInitParam& param, const SWindowInfo& wndInfo)
{
	initGlobal();

	g_Engine = new Engine(param, wndInfo);
}

void destroyEngine()
{
	delete g_Engine;
	g_Engine = NULL_PTR;

	deleteGlobal();
}