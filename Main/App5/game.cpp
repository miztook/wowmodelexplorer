#include "game.h"

bool g_bExit = false;
bool g_bBackMode = false;
IM2SceneNode* g_DeathWingSceneNode = NULL;

void MyMessageHandler::onSize(window_type hwnd, int width, int height)
{
	if (width == 0 || height == 0)					//minimized, stop
	{ 
		g_bBackMode = true;
	}
	else
	{
		g_bBackMode = false;
		IVideoDriver* driver = g_Engine->getDriver();
		if (driver)
			driver->setDisplayMode( dimension2du((u32)width, (u32)height) );

		IFontServices* fontServices = g_Engine->getFontServices();
		if (fontServices)
			fontServices->onWindowSizeChanged( dimension2du((u32)width, (u32)height) );

		ISceneManager* smgr = g_Engine->getSceneManager();
		if(smgr)
		{
			smgr->onWindowSizeChanged( dimension2du((u32)width, (u32)height) );

			ICamera* cam = smgr->getActiveCamera();
			if(cam)
				cam->recalculateAll();
		}
	}
}


void processResource()
{
	IResourceLoader* loader = g_Engine->getResourceLoader();
	if(loader->m2LoadCompleted())
	{
		IResourceLoader::STask task = loader->getCurrentTask();
		_ASSERT(task.type == IResourceLoader::ET_M2);
		IFileM2* filem2 = (IFileM2*)task.file;
		
		if (!filem2)
			return;

		loader->clearCurrentTask();

		filem2->buildVideoResources();

		loader->resumeLoading();

		IM2SceneNode*  node = g_Engine->getSceneManager()->addM2SceneNode(filem2, NULL);
		if (node)
		{
			node->buildVisibleGeosets();
	
			node->playAnimationByName("Stand", 0, true);
			node->setModelCamera(0);
		}

		g_DeathWingSceneNode = node;
	}
}

void createScene()
{
	/*
	g_Engine->getManualMeshServices()->addGridLineMesh("$grid20", 20, 1,SColor(128,128,128) );

	IMeshSceneNode* gridNode = g_Engine->getSceneManager()->addMeshSceneNode("$grid20", NULL);

	IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\HUMAN\\MALE\\HumanMale.m2");
	//IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\PANDAREN\\MALE\\PandarenMale.m2");
	IM2SceneNode*  humanNode = g_Engine->getSceneManager()->addM2SceneNode(m2Human, NULL);	
	//humanNode->getM2Appearance()->loadStartOutfit(446, false, true);
	//humanNode->getM2Appearance()->loadSet(935);		//631 //699 //1128 //1165 //2096 //622
	humanNode->getM2Appearance()->loadSet(1128, true);
	//	humanNode->getM2Appearance()->loadSet(699, true);
	//humanNode->getM2Appearance()->takeItem(95475);		//32944
	//humanNode->getM2Appearance()->takeItem(30732, true);			//30732
	humanNode->updateCharacter();
	humanNode->getM2Move()->setDir(-vector3df::UnitZ());

	g_Node = humanNode;
	*/

	IResourceLoader::SParamBlock block = {0};
	block.param1 = NULL;

	g_Engine->getResourceLoader()->beginLoadM2(
		"interface\\Glues\\Models\\UI_MainMenu_Cataclysm\\UI_MainMenu_CataClysm.m2",
		block);
}

void destroyScene()
{
	g_Engine->getSceneManager()->removeAllSceneNodes();
}

void idleTick()
{
	if (g_DeathWingSceneNode)
	{
		s16 idx = g_DeathWingSceneNode->getFileM2()->getAnimationIndex("Stand", 0);
		if (idx != -1)
		{
			animation anim;
			s32 current = g_DeathWingSceneNode->getCurrentAnimation(&anim);
			if (current != idx && anim.isFrameEnd())
				g_DeathWingSceneNode->playAnimationByIndex(idx, true, 200);
		}
	}
}
