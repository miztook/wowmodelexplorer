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
// 	const c8* url = "http://www.battlenet.com.cn/wow/zh/character/%E8%89%BE%E9%9C%B2%E6%81%A9/%E5%B0%8F%E7%9F%B3%E5%A4%B4%E5%86%B2%E5%95%8A/simple";
// 	CSysUtility::openURLtoJsonFile(url, "test.json");

	g_Engine->getManualMeshServices()->addGridLineMesh("$grid20", 20, 1,SColor(128,128,128) );
//	g_Engine->getManualMeshServices()->addDecal("$decal", 3, 3, SColor::Red());

	IMeshSceneNode* gridNode = g_Engine->getSceneManager()->addMeshSceneNode("$grid20", NULL);

//	IMeshSceneNode* decalNode = g_Engine->getSceneManager()->addMeshSceneNode("$decal", NULL);
	/*
	for (int i = -1; i < 1; i+=4)
	{
	 	for (int k = -1; k < 1; k+=4)
	 	{
			int npcid = 5562; //1405; //17765; //1405;	//34274; //525; //8783; //;//11460; 
			c8 path[256];
			g_Engine->getWowDatabase()->getNpcPath(npcid, false, path, 256);
			IFileM2* m2Lich = g_Engine->getResourceLoader()->loadM2(path);
			IM2SceneNode*  lichNode = g_Engine->getSceneManager()->addM2SceneNode(m2Lich, NULL, true);
			lichNode->getM2Move()->setDir(-vector3df::UnitZ());
			lichNode->getM2Move()->setPos(vector3df(i, 0, k));
			lichNode->updateNpc(npcid);
			lichNode->playAnimationByName("Stand", 0, true);
			//lichNode->setModelAlpha(true, 0.5f);
		}
	}
		*/
	{
		IFileM2* m2PandarenMale = g_Engine->getResourceLoader()->loadM2("Character\\SCOURGE\\MALE\\ScourgeMale_HD.m2");
		IM2SceneNode*  pandarenMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2PandarenMale, NULL);	
		pandarenMaleNode->getM2Instance()->CharacterInfo->HairStyle = 1;
		pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 0;	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->HairColor = 4;
		//pandarenMaleNode->getM2Appearance()->loadStartOutfit(448, false, true);
		pandarenMaleNode->getM2Appearance()->loadSet(1254, true);
		pandarenMaleNode->getM2Appearance()->takeItem(120394, false);
		pandarenMaleNode->getM2Appearance()->takeItem(119865, false);
		//pandarenMaleNode->getM2Appearance()->takeItem(116800, false);
		//pandarenMaleNode->getM2Appearance()->takeItem(867, false);
		//pandarenMaleNode->getM2Appearance()->showHelm(false);
		pandarenMaleNode->updateCharacter();
		
		pandarenMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		pandarenMaleNode->getM2FSM()->changeState(EMS_STAND);
	//	pandarenMaleNode->setDecalSceneNode(true);
	//	IDecalSceneNode* decalSceneNode = pandarenMaleNode->getDecalSceneNode();

	//	c8 tmp[512];
	//	cpptext_to_utf8("ÎÒ", tmp, 512);
	//	decalSceneNode->TextLine1 = tmp;
	}

	/*
	int filedataid = 1 * 256 * 256 + 200 * 256 + 186;
	char tmp[20];
	sprintf(tmp, "File%08X.unk", filedataid);
	bool bExsit = g_Engine->getWowEnvironment()->exists(tmp);
	IMemFile* pFile = g_Engine->getWowEnvironment()->openFile(tmp);
	pFile->read(tmp, 20);
	delete pFile;
	*/

	/*
	{
		IFileM2* m2PandarenMale = g_Engine->getResourceLoader()->loadM2("Character\\PANDAREN\\MALE\\PandarenMale.m2");
		IM2SceneNode*  pandarenMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2PandarenMale, NULL);	
		pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 1;	
		pandarenMaleNode->getM2Appearance()->loadStartOutfit(446, false, true);
		pandarenMaleNode->getM2Appearance()->takeItem(71352, false);
		pandarenMaleNode->getM2Appearance()->takeItem(103872, true);
		pandarenMaleNode->updateCharacter();
		pandarenMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		pandarenMaleNode->getM2Move()->setPos(vector3df(-2, 0, 0));
		pandarenMaleNode->getM2FSM()->changeState(EMS_STAND);
		pandarenMaleNode->setDecalSceneNode(true);
	}

	{
		IFileM2* m2PandarenMale = g_Engine->getResourceLoader()->loadM2("Character\\PANDAREN\\MALE\\PandarenMale.m2");
		IM2SceneNode*  pandarenMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2PandarenMale, NULL);	
		pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		pandarenMaleNode->getM2Appearance()->loadStartOutfit(446, false, true);
		pandarenMaleNode->getM2Appearance()->takeItem(71086, false);
		pandarenMaleNode->getM2Appearance()->takeItem(105326, false);
		pandarenMaleNode->updateCharacter();
		pandarenMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		pandarenMaleNode->getM2Move()->setPos(vector3df(2, 0, 0));
		pandarenMaleNode->getM2FSM()->changeState(EMS_STAND);
		pandarenMaleNode->setDecalSceneNode(true);
	}
	*/

// 	const c8* path = "World\\wmo\\Northrend\\Dalaran\\ND_Dalaran.wmo";
// 	IFileWMO* wmo = g_Engine->getResourceLoader()->loadWMO(path);
// 	IWMOSceneNode* wmoSceneNode = g_Engine->getSceneManager()->addWMOSceneNode(wmo, NULL);
// 	matrix4 mat;
// 	mat.setScale(0.5f);
// 	wmoSceneNode->setRelativeTransformation(mat);
}

void destroyScene()
{
	g_Engine->getSceneManager()->removeAllSceneNodes();
}

void idleTick()
{

}

