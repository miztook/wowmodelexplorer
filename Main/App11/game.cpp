#include "game.h"

bool g_bExit = false;
bool g_bBackMode = false;

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


void createScene()
{
	g_Engine->getManualMeshServices()->addGridLineMesh("$grid20", 20, 1,SColor(128,128,128) );

	IMeshSceneNode* gridNode = g_Engine->getSceneManager()->addMeshSceneNode("$grid20", NULL);

	for (int i = -10; i < 10; i+=4)
	{
		for (int k = -10; k < 10; k+=4)
		{
			int npcid = 1405; //1405; //525; //8783; //17765;//11460; 
			c8 path[256];
			g_Engine->getWowDatabase()->getNpcPath(npcid, true, path, 256);
			IFileM2* m2Lich = g_Engine->getResourceLoader()->loadM2(path);
			IM2SceneNode*  lichNode = g_Engine->getSceneManager()->addM2SceneNode(m2Lich, NULL, true);
			lichNode->getM2Move()->setDir(-vector3df::UnitZ());
			lichNode->getM2Move()->setPos(vector3df(i, 0, k));
			lichNode->updateNpc(npcid);
			lichNode->getM2FSM()->changeState(EMS_STAND);
			//	lichNode->setModelAlpha(true, 0.5f);
		}
	}
}

void destroyScene()
{
	g_Engine->getSceneManager()->removeAllSceneNodes();
}

void idleTick()
{
}
