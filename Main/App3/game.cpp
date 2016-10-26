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
	string64 act = "Stand";

	/*
	{
		IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\HUMAN\\MALE\\HumanMale.m2");
		//IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\PANDAREN\\MALE\\PandarenMale.m2");
		IM2SceneNode*  humanNode = g_Engine->getSceneManager()->addM2SceneNode(m2Human, NULL);	
		//humanNode->getM2Appearance()->loadStartOutfit(446, false, true);
		//	humanNode->getM2Appearance()->loadSet(1189);		//631 //699 //1128 //1165 //2096 //622
		//	humanNode->getM2Appearance()->loadSet(671, true);
		//	humanNode->getM2Appearance()->loadSet(699, true);
		//humanNode->getM2Appearance()->takeItem(95475);		//32944
	//	humanNode->getM2Appearance()->takeItem(34185, true);			//30732
	//	humanNode->getM2Appearance()->takeItem(32375);
		//	humanNode->getM2Appearance()->takeItem(73211, NULL, true);
		humanNode->updateCharacter();
		humanNode->getM2Move()->setDir(-vector3df::UnitZ());
		humanNode->getM2FSM()->changeState(EMS_STAND);

		g_M2SceneNode = humanNode;
	}
	*/
	
    
 	for (int i = -10; i < 10; i+=4)
 	{
 		for (int k = -10; k < 10; k+=4)
		{
			int npcid = 17765; //1405; //525; //8783; //17765;//11460; 
			c8 path[256];
			g_Engine->getWowDatabase()->getNpcPath(npcid, false, path, 256);
			IFileM2* m2Lich = g_Engine->getResourceLoader()->loadM2(path);
			IM2SceneNode*  lichNode = g_Engine->getSceneManager()->addM2SceneNode(m2Lich, NULL, true);
			matrix4 mat;
			mat.setRotationDegrees(vector3df(0,90,0));
			//mat.setScale(0.2f);
			//mat.setTranslation(vector3df(5.0f, 0.0f,0.0f));
			mat.setTranslation(vector3df(i, 0, k));
			lichNode->setRelativeTransformation(mat);
			lichNode->updateNpc(npcid);
			lichNode->playAnimationByName("Stand", 0, true);
			//	lichNode->setModelAlpha(true, 0.5f);
 		}
	}
    
 
// 		if (lichNode->setMountM2SceneNode(g_M2SceneNode))
// 		{
// 			g_M2SceneNode->getM2FSM()->changeState(EMS_MOUNT);
// 		}
		
}

void destroyScene()
{
	g_Engine->getSceneManager()->removeAllSceneNodes();
}

void idleTick()
{

}
