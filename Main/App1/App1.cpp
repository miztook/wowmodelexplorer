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

	//_CrtSetBreakAlloc(499943);

	CMemDbg globalDbg;
	globalDbg.beginCheckPoint();
	
//	globalDbg.setAllocHook(true, 0);

	SWindowInfo wndInfo = Engine::createWindow("App1", dimension2du(1136,640));
	HWND hwnd = wndInfo.hwnd;

	SEngineInitParam param;
	createEngine(param, wndInfo);

	bool success = g_Engine->initDriver(EDT_OPENGL, 0, false, true, 1, true);
	g_bExit = !success;

	MyMessageHandler handler;
	g_Engine->setMessageHandler(&handler);

	//g_Engine->getWowDatabase()->buildItems();
	//g_Engine->getWowDatabase()->buildNpcs("npcs.csv");
	g_Engine->getWowDatabase()->buildMaps();

	g_Engine->initSceneManager();

	createClient();

	//ISound* sound = g_Engine->getAudioPlayer()->getEmptyMp3Sound();
	//sound->load("Sound\\music\\cataclysm\\MUS_Shattering_UU01.mp3");
	//sound->load("Sound\\music\\GlueScreenMusic\\WotLK_main_title.mp3");
	//sound->load("Sound\\music\\CityMusic\\Stormwind\\stormwind_intro-moment.mp3");
	//sound->play(false);

	ISceneManager* sceneMgr = g_Engine->getSceneManager();

	ICamera* cam = sceneMgr->addCamera(vector3df(0, 5, -10), vector3df(0,0,0), vector3df(0,1,0), 1.0f, 2500.0f, PI/4.0f);
	
	IPlayer* player = g_Client->getPlayer();
	player->setTargetCamera(1.0f, 2500.0f, PI/4.0f);
	g_Engine->getSceneManager()->setActiveCamera(player->getTargetCamera()->getCamera());

	createScene();

	player->setM2AsTarget(g_Node, 15, PI/6);

	createInput();

	u32 lastTime = g_Engine->getTimer()->getTimeSinceStart();

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
			u32 now = g_Engine->getTimer()->getTimeSinceStart();
			u32 time =  min_(now - lastTime, 500u);
			lastTime = now;

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

				g_Client->tick(time);
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

