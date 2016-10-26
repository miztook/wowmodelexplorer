#include "game.h"

bool g_bExit = false;
bool g_bBackMode = false;

IM2SceneNode* g_Node = NULL;
ICoordSceneNode* g_Coord = NULL;
IFileWDT* fileWDT = NULL;

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
	//add static mesh
	g_Engine->getManualMeshServices()->addGridLineMesh("$grid20", 20, 1,SColor(128,128,128) );
//	g_Engine->getManualMeshServices()->addSphere("$sphere8X4", 1.0f, 8, 4, SColor(255,255,0) );
//	g_Engine->getManualMeshServices()->addDecal("$decal", 1, 2, SColor(128, 128, 128));
//	g_Engine->getManualMeshServices()->addSkyDome("$skydome", 10, 6, 1.5, 2, SColor(255,255,0));

	const SMapRecord* mapRecord = g_Engine->getWowDatabase()->getMap(1);

// 	for (u32 i=0; i<mapRecord->areaList.size(); ++i)
// 	{
// 		SArea* area = g_Engine->getWowDatabase()->getAreaById(mapRecord->areaList[i]);
//  		if (area->parentId == 0)
//  			MessageBoxW(NULL, area->name, L"", 0);
// 	}
	//

	g_Engine->getEngineSetting()->setViewDistance(EL_FAIR);

	c8 mapname[MAX_PATH];
	sprintf_s(mapname, MAX_PATH, "World\\Maps\\%s\\%s.wdt", mapRecord->name, mapRecord->name);
	//sprintf_s(mapname, MAX_PATH, "World\\Maps\\%s\\%s.wdt", "StormwindJail", "StormwindJail");
	fileWDT = g_Engine->getResourceLoader()->loadWDT(mapname, mapRecord->id);
	if (!fileWDT)
		return;

//		IWDTSceneNode* node = g_Engine->getSceneManager()->addWDTSceneNode(fileWDT, NULL);
//		node->setCurrentTile(29, 40);
// 	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
// 	cam->Position = node->getCenter();
// 	cam->recalculateAll();


	STile* tile = fileWDT->getTile(29,40);
	_ASSERT(tile);
	IMapTileSceneNode* mapTileNode = g_Engine->getSceneManager()->addMapTileSceneNode(fileWDT, tile, NULL);

	g_Engine->getSceneManager()->addSkySceneNode(fileWDT->getMapEnvironment());

	//mapTileNode->addM2SceneNodes();
	//mapTileNode->addWMOSceneNodes();

	/*	matrix4 mat;
	mat.setTranslation(-node->getCenter());
	node->setRelativeTransformation(mat);
	//node->addWMOSceneNodes();
	node->startLoadingWMOSceneNodes();
	node->startLoadingM2SceneNodes();
	//node->addM2SceneNodes();

	const c8* path = "World\\wmo\\Northrend\\Dalaran\\ND_Dalaran.wmo";
//	const c8* path ="world\\wmo\\Dungeon\\IceCrown_dungeon\\Icecrown_Halls_of_reflection.wmo";
//	const c8* path = "WORLD\\WMO\\AZEROTH\\BUILDINGS\\STORMWIND\\STORMWIND.WMO";
//	const c8* path = "world\\wmo\\kalimdor\\ogrimmar\\orgrimmar2.wmo";
	IFileWMO* wmo = g_Engine->getResourceLoader()->loadWMO(path);
	IWMOSceneNode* wmoSceneNode = g_Engine->getSceneManager()->addWMOSceneNode(wmo, NULL);
	
// 	matrix4 mat;
// 	mat.setScale(0.5f);
// 	wmoSceneNode->setRelativeTransformation(mat);
*/	/*	
	IResourceLoader::SParamBlock block = {0};
	g_Engine->getResourceLoader()->beginLoadM2("Interface\\Glues\\Models\\UI_MainMenu_Cataclysm\\UI_MainMenu_Cataclysm.m2", block);


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

		g_Node = nagaNode;
	}
		*/

	//add scene node
	//IMeshSceneNode* gridNode = g_Engine->getSceneManager()->addMeshSceneNode("$grid20", NULL);
//	IMeshSceneNode* skyDomeNode = g_Engine->getSceneManager()->addMeshSceneNode("$skydome", NULL);
	//string64 act = "Stand";
/*
	ICoordSceneNode* defaultCoord = g_Engine->getSceneManager()->addCoordSceneNode(NULL);
	defaultCoord->setPosition2D(ICoordSceneNode::EP2D_BOTTOMLEFT);
	defaultCoord->setAxisColor(SColor::Red(), SColor::Green(), SColor::Blue());
	defaultCoord->setAxisText("x", "y", "z");
*/
/*
	g_Coord = g_Engine->getSceneManager()->addCoordSceneNode();
	g_Coord->setPosition2D(ICoordSceneNode::EP2D_TOPCENTER);
	g_Coord->setAxisVisible(false, false, true);
	g_Coord->setAxisColor(SColor::Red(), SColor::Green(), SColor::White());
	g_Coord->setAxisText("", "", "light");
	g_Coord->setDir(ICoordSceneNode::ESA_Z, g_Engine->getSceneEnvironment()->LightDir);
*/
	g_Client->getWorld()->setWorldSceneNode(mapTileNode);
	vector3df pos = mapTileNode->getCenter();

	//alliance
	{
		//for (int i = -20; i < 20; i+=4)
		{
			//for (int k = -20; k < 20; k+=4)
			{
				IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\PANDAREN\\MALE\\PandarenMale.m2");
				IM2SceneNode*  humanNode = g_Engine->getSceneManager()->addM2SceneNode(m2Human, NULL);	
			//	humanNode->getM2Appearance()->loadStartOutfit(446, false, true);
			//	humanNode->getM2Appearance()->loadSet(213);		//631 //699 //1128 //1165 //2096 //622
			//	humanNode->getM2Appearance()->loadSet(896, true);
			//	humanNode->getM2Appearance()->loadSet(699, true);
			//	humanNode->getM2Appearance()->takeItem(77509, NULL);
			//	humanNode->getM2Appearance()->takeItem(73211, NULL, true);
				humanNode->updateCharacter();
			//	mat.setTranslation(vector3df(i, 0, k));
				humanNode->getM2Move()->setPos(pos);
				humanNode->getM2Move()->setDir(-vector3df::UnitZ());
				//humanNode->setRelativeTransformation(mat);
			//	humanNode->playAnimationByName(act.c_str(), 0, true);
			//	humanNode->playAnimationByIndex(0, true);
			//	humanNode->setModelCamera(0);
			//	humanNode->getM2FSM()->changeState(EMS_STAND);

				g_Node = humanNode;
			}
		}
	}

}

void destroyScene()
{
	if(fileWDT)
		fileWDT->drop();
}

void idleTick()
{

}

