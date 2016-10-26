#include "mywow.h"
#include "CMiniDump.h"
#include "CMemDbg.h"
#include "game.h"
#include "mywowclient.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "mywowclient.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

int main()
{
	CMiniDump::begin();

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//_CrtSetBreakAlloc(115);

	CMemDbg globalDbg;
	globalDbg.beginCheckPoint();

	SWindowInfo wndInfo = Engine::createWindow("App4", dimension2du(1136,640));
	HWND hwnd = wndInfo.hwnd;

	SEngineInitParam param;	
	//param.useMpq = false;
	//param.outputFileName = true;

	createEngine(param, wndInfo);

	bool success = g_Engine->initDriver(EDT_OPENGL, 0, false, true, 1, true);
	g_bExit = !success;

	//test
	{
		std::vector<string_cs256> vFiles;
		std::vector<string_cs256> vDirs;
		//g_Engine->getWowEnvironment()->getFiles("DBFilesClient", "dbc", vFiles);
		//g_Engine->getWowEnvironment()->getDirectories("character/bloodelf", vDirs, false);
	}

	MyMessageHandler handler;
	g_Engine->setMessageHandler(&handler);

	g_Engine->initSceneManager();

	//g_Engine->getWowDatabase()->buildNpcs("npcs.csv");
	g_Engine->getWowDatabase()->buildItems();
	//g_Engine->getWowDatabase()->buildMaps();
	//g_Engine->getWowDatabase()->buildWmos();
	//g_Engine->getWowDatabase()->buildStartOutfitClass();
	//g_Engine->getWowDatabase()->buildTextures();

	//u32 num = g_Engine->getWowDatabase()->getNumTextures();

	ISceneManager* sceneMgr = g_Engine->getSceneManager();

	createClient();

	ICamera* cam = sceneMgr->addCamera(vector3df(0, 5, -10), vector3df(0,1,0), vector3df(0,1,0), 1.0f, 2500.0f, PI/4.0f);

	createScene();

	createInput();

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

	destroyEngine();

	bool safe = globalDbg.endCheckPoint();
	_ASSERT(safe);

	globalDbg.outputMaxMemoryUsed();

	CMiniDump::end();

	return 0;
}
