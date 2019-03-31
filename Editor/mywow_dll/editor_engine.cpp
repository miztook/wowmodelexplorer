#include "stdafx.h"
#include "editor_engine.h"
#include "CEditSceneManager.h"
#include "CMiniDump.h"
#include "editor_WowEnvironment.h"

_CrtMemState oldState;
_CrtMemState newState;
_CrtMemState diffState;

void  Engine_create()
{
	CMiniDump::begin();

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	_CrtMemCheckpoint(&oldState);

	SWindowInfo wndInfo = Engine::createWindow("", dimension2du(40,30), 1.0f, false, true);

	SEngineInitParam param;
	createEngine(param, wndInfo);
}

void  Engine_destroy()
{
	WowEnvironment_Clear();

	destroyEngine();

	_CrtMemCheckpoint(&newState);

	int32_t diff = _CrtMemDifference(&diffState, &oldState, &newState);
	_ASSERT(diff == 0);

	CMiniDump::end();
}

HWND Engine_getHWnd()
{
	return g_Engine->getWindowInfo().hwnd;
}

bool  Engine_initDriver( E_DRIVER_TYPE driverType, uint32_t adapter, bool fullscreen, bool vsync, uint32_t antialias, bool multithread )
{
	bool ret = g_Engine->initDriver(driverType, adapter, fullscreen, vsync, (uint8_t)antialias, multithread);

	return ret;
}

void  Engine_initSceneManager()
{
	CEditSceneManager* mgr = new CEditSceneManager;
	g_Engine->setSceneManager(mgr);
}

IVideoDriver*  Engine_getDriver()
{
	return g_Engine->getDriver();
}

ISceneManager*  Engine_getSceneManager()
{
	return g_Engine->getSceneManager();
}

WowClassic::wowDatabase*  Engine_getWowDatabase()
{
	return g_Engine->getWowDatabase();
}

wowEnvironment*  Engine_getWowEnvironment()
{
	return g_Engine->getWowEnvironment();
}

IManualMeshServices*  Engine_getManualMeshServices()
{
	return g_Engine->getManualMeshServices();
}

IResourceLoader* Engine_getResourceLoader()
{
	return g_Engine->getResourceLoader();
}

ISceneEnvironment* Engine_getSceneEnvironment()
{
	return g_Engine->getSceneEnvironment();
}

IDrawServices* Engine_getDrawServices()
{
	return g_Engine->getDrawServices();
}

IFontServices* Engine_getFontServices()
{
	return g_Engine->getFontServices();
}

void Engine_getBaseDirectory(char* dirname, uint32_t size)
{
	const char* baseDir = g_Engine->getFileSystem()->getBaseDirectory();
	g_Engine->getFileSystem()->getAbsolutePath(baseDir, dirname, size);
}

void Engine_applyVideoSetting( engineSetting::SVideoSetting videosetting )
{
	g_Engine->getEngineSetting()->applyVideoSetting(videosetting);
}

void Engine_getVideoSetting( engineSetting::SVideoSetting* videosetting )
{
	*videosetting = g_Engine->getEngineSetting()->getVideoSetting();
}

void Engine_applyAdvancedSetting( engineSetting::SAdvancedSetting advancedSetting )
{
	g_Engine->getEngineSetting()->applyAdvancedSetting(advancedSetting);
}

void Engine_getAdvancedSetting( engineSetting::SAdvancedSetting* advancedSetting )
{
	*advancedSetting = g_Engine->getEngineSetting()->getAdvcanedSetting();
}

void Engine_loadSetting()
{
	g_Engine->getEngineSetting()->load();
}

void Engine_saveSetting()
{
	g_Engine->getEngineSetting()->save();
}

bool Engine_isAboveVista()
{
	return g_Engine->getOSInfo()->IsAeroSupport();
}


