#include "mywow.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

#define IS_KEY_PRESSED(vk) ( ( GetAsyncKeyState(vk) & 0x8000 ) != 0 )

bool bExit = false;
bool bStop = false;
HWND g_hWnd;

void cameraOnEvent(ICamera* cam, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_QUIT || message == WM_CLOSE ||
		( message == WM_KEYDOWN && wParam == VK_ESCAPE ))
		bExit = true;

	if (!bExit)
	{
		if ( message == WM_SIZE)
		{
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);

			if (width == 0 || height == 0)					//minimized, stop
			{ 
				bStop = true;
			}
			else
			{
				bStop = false;
				IVideoDriver* driver = g_Engine->getDriver();
				if (driver)
					driver->setDisplayMode( dimension2du((u32)width, (u32)height) );
			}
		}

		ISceneManager* smgr = g_Engine->getSceneManager();
		if (smgr)
		{
			ICamera* cam = smgr->getActiveCamera();
			if(cam)
				cameraOnEvent(cam, hWnd, message, wParam, lParam);
		}

	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}

f32		LastX = 0;
f32		LastY = 0;
bool		LCapture = false;
bool		RCapture = false;

void cameraOnEvent(ICamera* cam, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		cam->recalculateAll();
		break;
	case WM_LBUTTONDOWN:
		{
			if (!RCapture)
			{
				LastX = (float)(short)LOWORD( lParam );
				LastY = (float)(short)HIWORD( lParam );
				SetCapture( hWnd );
				LCapture = true;
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			if (LCapture)
			{
				LastX = LastY = 0;
				ReleaseCapture();
				LCapture = false;
			}	
		}
		break;
	case WM_RBUTTONDOWN:
		{
			if (!LCapture)
			{
				LastX = (float)(short)LOWORD( lParam );
				LastY = (float)(short)HIWORD( lParam );
				SetCapture( hWnd );
				RCapture = true;
			}
		}
		break;
	case WM_RBUTTONUP:
		{
			if (RCapture)
			{
				LastX = LastY = 0;
				ReleaseCapture();
				RCapture = false;
			}	
		}
		break;
	case WM_MOUSEMOVE:
		{
			float x = (float)(short)LOWORD( lParam );
			float y = (float)(short)HIWORD( lParam );

			const float blend = 0.8f;				//Ðý×ªÈáºÍ

			if (LCapture || RCapture)
			{
				float deltaX = x - LastX;
				float deltaY = y - LastY;

				bool xChange = fabs(deltaX) > 0.5f;
				bool yChange = fabs(deltaY) > 0.5f;

				if (  xChange || yChange  )
				{
					float blendDeltaX = xChange ? deltaX * blend : deltaX;
					float blendDeltaY = yChange ? deltaY * blend : deltaY;

  					if (LCapture)			
  						cam->pitch_yaw_Maya( blendDeltaY * 0.3f, blendDeltaX * 0.3f );			
  					else
  						cam->move_offset_Maya( blendDeltaX * 0.02f, blendDeltaY * 0.02f);

//					cam->pitch_yaw_FPS(blendDeltaY * 0.2f, blendDeltaX * 0.2f);

					LastX += blendDeltaX;
					LastY += blendDeltaY;
				}
			}
		}
		break;
	default:
		break;
	}
}


void createScene();
void destroyScene();
void processResource();

int main()
{
// #if defined(DEBUG) | defined(_DEBUG)
 //	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
// #endif

	//_CrtSetBreakAlloc(5953);

	createEngine();

	//parse bls
	//blsParser::parseAll();

	g_Engine->getWowDatabase()->buildItems("items.csv");
	g_Engine->getWowDatabase()->buildNpcs("npcs.csv");
//	g_Engine->getWowDatabase()->buildMaps();

 	g_hWnd = Engine::createWindow("App1", WndProc, dimension2du(800,600));
	bool success = g_Engine->initDriver(g_hWnd, EDT_DIRECT3D9, false, true, true, 32, 1);
	bExit = !success;

	g_Engine->initSceneManager();

	ISceneManager* sceneMgr = g_Engine->getSceneManager();

	sceneMgr->addCamera(vector3df(0,5,-10), vector3df(0,0,0), vector3df(0,1,0), 1.0f, 500.0f, PI/4.0f);

	createScene();

	MSG msg;
	while (!bExit)	
	{
		if ( ::PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage(&msg);
		}
		else
		{
			if (!bStop && ::GetActiveWindow() == g_hWnd)
			{
				ICamera* cam = sceneMgr->getActiveCamera();
				ICamera::SKeyControl keyControl;
				keyControl.front = IS_KEY_PRESSED('W');
				keyControl.back = IS_KEY_PRESSED('S');
				keyControl.left = IS_KEY_PRESSED('A');
				keyControl.right = IS_KEY_PRESSED('D');
				keyControl.up = IS_KEY_PRESSED(VK_SPACE);
				keyControl.down = false;

				if (cam)
				{
					cam->onKeyMove(0.4f, keyControl);
				}

				processResource();

				sceneMgr->drawAll();
			}
			::Sleep(1);
		}
	}


	destroyScene();

	destroyEngine();

	return 0;
}

void processResource()
{
	IResourceLoader* loader = g_Engine->getResourceLoader();
	if(loader->m2LoadCompleted())
	{
		IResourceLoader::STask task = loader->getCurrentM2Task();
		IFileM2* filem2 = (IFileM2*)task.file;

		loader->resumeLoadingM2();

		IM2SceneNode*  node = g_Engine->getSceneManager()->addM2SceneNode(filem2, NULL);
		if (node)
		{
			node->buildVisibleGeosets();

			node->playAnimationByName("Stand", 0, true);
			node->setModelCamera(0);
		}
	}
}

IM2SceneNode* mainNode;
IFileWDT*	fileWDT = NULL;

void createScene()
{
	//add static mesh
	g_Engine->getManualMeshServices()->addGridLineMesh("$grid10", 10, 1,SColor(128,128,128) );


	g_Engine->getResourceLoader()->beginLoadM2("Interface\\Glues\\Models\\UI_MainMenu_Cataclysm\\UI_MainMenu_CataClysm.m2");
	
/*
	{	
	
		//UI_MainMenu_Cataclysm\\UI_MainMenu_CataClysm.m2
		IFileM2* m2Naga  = g_Engine->getResourceLoader()->loadM2("Interface\\Glues\\Models\\UI_MainMenu_Cataclysm\\UI_MainMenu_CataClysm.m2");//("Spells\\Holy_Precast_Uber_Hand.M2");

	//	IFileM2* m2Naga  = g_Engine->getResourceLoader()->loadM2("Spells\\Cyclone_State.M2");
		IM2SceneNode*  nagaNode = g_Engine->getSceneManager()->addM2SceneNode(m2Naga, NULL);
		nagaNode->buildVisibleGeosets();
		matrix4 mat;
//		mat.setRotationDegrees(vector3df(0,90,0));
			nagaNode->setRelativeTransformation(mat);
		nagaNode->playAnimationByName("Stand", 0, true);
		nagaNode->setModelCamera(0);
	}
	*/

	//add scene node
//	IMeshSceneNode* gridNode = g_Engine->getSceneManager()->addMeshSceneNode("$grid10", NULL);

	string64 act = "Stand";

/*
	//alliance
	{
		IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\HUMAN\\MALE\\HumanMale.m2");
		IM2SceneNode*  humanNode = g_Engine->getSceneManager()->addM2SceneNode(m2Human, NULL);	
	//	humanNode->loadSet(679);		//631
	//	humanNode->loadStartOutfit(352, true);
		humanNode->updateCharacter();
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(0, 0, 0));
		humanNode->setRelativeTransformation(mat);
		humanNode->playAnimationByName(act.c_str(), 0, true);
		//humanNode->setModelCamera(1);
		
		humanNode->takeItem(48695, NULL);
	}

	{
		IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\HUMAN\\MALE\\HumanMale.m2");
		IM2SceneNode*  humanNode = g_Engine->getSceneManager()->addM2SceneNode(m2Human, NULL);	
		humanNode->loadSet(631);		//631
		//humanNode->loadStartOutfit(352, true);
		humanNode->updateCharacter();
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		//mat.setTranslation(vector3df(-1.5f, 0, 0));
		humanNode->setRelativeTransformation(mat);
		humanNode->playAnimationByName(act.c_str(), 0, true);
		//humanNode->setModelCamera(1);
		//humanNode->loadSet(518);
		//humanNode->takeItem(32837);
	}
*/
	/*	
	{
		IFileM2* m2Worgen = g_Engine->getResourceLoader()->loadM2("Character\\WORGEN\\MALE\\WorgenMale.m2");
		IM2SceneNode*  worgenNode = g_Engine->getSceneManager()->addM2SceneNode(m2Worgen, NULL);
		worgenNode->loadSet(518);
		worgenNode->updateCharacter();
		
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(-3, 0, 0));
		worgenNode->setRelativeTransformation(mat);
		worgenNode->playAnimationByName(act.c_str(), 0, true);
	}
	{
		IFileM2* m2Draenei = g_Engine->getResourceLoader()->loadM2("Character\\DRAENEI\\MALE\\DraeneiMale.m2");
		IM2SceneNode*  draeneiNode = g_Engine->getSceneManager()->addM2SceneNode(m2Draenei, NULL);
		draeneiNode->loadSet(631);
		draeneiNode->updateCharacter();

		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(-4.5f, 0, 0));
		draeneiNode->setRelativeTransformation(mat);
		draeneiNode->playAnimationByName(act.c_str(), 0, true);
	}


	{
		IFileM2* m2Dwarf= g_Engine->getResourceLoader()->loadM2("Character\\DWARF\\FEMALE\\DwarfFemale.m2");
		IM2SceneNode*  dwarfNode = g_Engine->getSceneManager()->addM2SceneNode(m2Dwarf, NULL);
		dwarfNode->loadSet(631);
		dwarfNode->updateCharacter();

		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(-6, 0, 0));
		dwarfNode->setRelativeTransformation(mat);
		dwarfNode->playAnimationByName(act.c_str(), 0, true);
	}
	
{
		IFileM2* m2Gnome= g_Engine->getResourceLoader()->loadM2("Character\\GNOME\\MALE\\GnomeMale.m2");
		IM2SceneNode*  gnomeNode = g_Engine->getSceneManager()->addM2SceneNode(m2Gnome, NULL);
		//gnomeNode->loadSet(631);
		gnomeNode->getWowCharacter()->FacialHair = 1;
		gnomeNode->updateCharacter();

		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(0, 0, 0));
		gnomeNode->setRelativeTransformation(mat);
		gnomeNode->playAnimationByName(act.c_str(), 0, true);
	}
*/	
	/*
	{
		IFileM2* m2NightElf= g_Engine->getResourceLoader()->loadM2("Character\\NIGHTELF\\MALE\\NightElfMale.m2");
		IM2SceneNode*  nightElfNode = g_Engine->getSceneManager()->addM2SceneNode(m2NightElf, NULL);
		nightElfNode->loadSet(631);
		nightElfNode->updateCharacter();
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(-9, 0, 0));
		nightElfNode->setRelativeTransformation(mat);
		
		nightElfNode->playAnimationByName(act.c_str(), 0, true);
	}

	//horde
	{
		IFileM2* m2Orc = g_Engine->getResourceLoader()->loadM2("Character\\ORC\\MALE\\OrcMale.m2");
		IM2SceneNode*  orcNode = g_Engine->getSceneManager()->addM2SceneNode(m2Orc, NULL);
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(1.5f, 0, 0));
		orcNode->loadSet(814);
		orcNode->setRelativeTransformation(mat);
		orcNode->playAnimationByName(act.c_str(), 1, true);
		orcNode->updateCharacter();
	}

	{
		IFileM2* m2Goblin = g_Engine->getResourceLoader()->loadM2("Character\\GOBLIN\\MALE\\GoblinMale.m2");
		IM2SceneNode*  goblinNode = g_Engine->getSceneManager()->addM2SceneNode(m2Goblin, NULL);
		goblinNode->loadSet(814);
		goblinNode->updateCharacter();
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(3.0f, 0, 0));
		goblinNode->setRelativeTransformation(mat);
		goblinNode->playAnimationByName(act.c_str(), 0, true);
	}

	{
		IFileM2* m2BloodElf = g_Engine->getResourceLoader()->loadM2("Character\\BLOODELF\\MALE\\BloodElfMale.m2");
		IM2SceneNode*  bloodelfNode = g_Engine->getSceneManager()->addM2SceneNode(m2BloodElf, NULL);
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(0, 0, 0));
	//	bloodelfNode->loadStartOutfit(364, true);
	//	bloodelfNode->loadSet(670);
		bloodelfNode->updateCharacter();
		bloodelfNode->setRelativeTransformation(mat);
		bloodelfNode->playAnimationByName(act.c_str(), 0, true);
		mainNode = bloodelfNode;
	}	

	{
		IFileM2* m2Scourge = g_Engine->getResourceLoader()->loadM2("Character\\SCOURGE\\MALE\\ScourgeMale.m2");
		IM2SceneNode*  scourgeNode = g_Engine->getSceneManager()->addM2SceneNode(m2Scourge, NULL);
		scourgeNode->loadSet(670);
		scourgeNode->updateCharacter();

		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(6, 0, 0));
		scourgeNode->setRelativeTransformation(mat);
		scourgeNode->playAnimationByName(act.c_str(), 0, true);
	}

	{
		IFileM2* m2Tauren = g_Engine->getResourceLoader()->loadM2("Character\\TAUREN\\MALE\\TaurenMale.m2");
		IM2SceneNode*  taurenNode = g_Engine->getSceneManager()->addM2SceneNode(m2Tauren, NULL);
		taurenNode->loadSet(670);
		taurenNode->updateCharacter();
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(7.5f, 0, 0));
		taurenNode->setRelativeTransformation(mat);
		taurenNode->playAnimationByName(act.c_str(), 0, true);
	}

	{
		IFileM2* m2Troll = g_Engine->getResourceLoader()->loadM2("Character\\TROLL\\MALE\\TrollMale.m2");
		IM2SceneNode*  trollNode = g_Engine->getSceneManager()->addM2SceneNode(m2Troll, NULL);
		trollNode->loadSet(670);
		trollNode->updateCharacter();
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		mat.setTranslation(vector3df(9, 0, 0));
		trollNode->setRelativeTransformation(mat);
		trollNode->playAnimationByName(act.c_str(), 0, true);
	}


	{
		//575 »ðÔªËØ
		//46471  ËÀÍöÖ®ÒíÈËÐÎ
		// 44988 ÈíÄà¹Ö
		// 92 Ê¯ÔªËØ
		// 52660 »ðÓ¥
		// 40625 ÐÇæí
		// 36911 ÐÜÃ¨ÈË
		int npcid = 24239; //21362; //11502; //510; //8542; //33113; //24248; //41453; //510; 
		c8 path[256];
		g_Engine->getWowDatabase()->getNpcPath(npcid, path, 256);
		IFileM2* m2Lich = g_Engine->getResourceLoader()->loadM2(path);
		IM2SceneNode*  lichNode = g_Engine->getSceneManager()->addM2SceneNode(m2Lich, NULL, true);
		matrix4 mat;
		mat.setRotationDegrees(vector3df(0,90,0));
		//mat.setScale(0.2f);
		mat.setTranslation(vector3df(0.0f, 0.0f,0.0f));
		lichNode->setRelativeTransformation(mat);
		lichNode->updateNpc(npcid);
		lichNode->playAnimationByName("Stand", 0, true);
	}
	*/
}

void destroyScene()
{
	g_Engine->getSceneManager()->removeAllSceneNodes();
}
