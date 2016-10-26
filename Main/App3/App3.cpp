#include "mywow.h"
#include "mywowclient.h"
#include "CMiniDump.h"
#include "CMemDbg.h"
#include "game.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "mywowclient.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

int main()
{
	CMiniDump::begin();

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//_CrtSetBreakAlloc(379762);

	CMemDbg globalDbg;
	globalDbg.beginCheckPoint();

	SWindowInfo wndInfo = Engine::createWindow("App3", dimension2du(1136,640));
	HWND hwnd = wndInfo.hwnd;

	SEngineInitParam param;
	//param.outputFileName = true;
	createEngine(param, wndInfo);

	engineSetting* setting = g_Engine->getEngineSetting();
	setting->load();

	bool success = g_Engine->initDriver(
		EDT_OPENGL,//setting->getAdvcanedSetting().driverType, 
		0, 
		false, 
		setting->getVideoSetting().vsync, 
		setting->getVideoSetting().antialias, 
		true);

	setting->applyVideoSetting(setting->getVideoSetting());
	setting->applyAdvancedSetting(setting->getAdvcanedSetting());

	g_bExit = !success;

	MyMessageHandler handler;
	g_Engine->setMessageHandler(&handler);

//	g_Engine->getWowDatabase()->buildItems();
//	g_Engine->getWowDatabase()->buildNpcs("npcs.csv");
//	g_Engine->getWowDatabase()->buildStartOutfitClass();

	g_Engine->initSceneManager();

	createClient();

	ISceneManager* sceneMgr = g_Engine->getSceneManager();

	ICamera* cam = sceneMgr->addCamera(vector3df(0, 5, -10), vector3df(0,0,0), vector3df(0,1,0), 1.0f, 2500.0f, PI/4.0f);

	createScene();

	createInput();

	u32 startTime = g_Engine->getTimer()->getTimeSinceStart();

	MSG msg;
	while (!g_bExit)	
	{
		if ( ::PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage(&msg);
		}
		else
		{
			bool active = !g_bBackMode && ::GetActiveWindow() == hwnd;
			if (active)
			{
				processInput();

				idleTick();
			}

			if (sceneMgr->beginFrame(active))
			{
				sceneMgr->drawAll(!g_bBackMode);

				sceneMgr->endFrame();
			}
			
			::Sleep(1);
		}
	}

	destroyInput();

	destroyScene();

	destroyClient();

	g_Engine->getEngineSetting()->save();

	destroyEngine();

	bool safe = globalDbg.endCheckPoint();
	_ASSERT(safe);

	globalDbg.outputMaxMemoryUsed();

	CMiniDump::end();

	return 0;
}
