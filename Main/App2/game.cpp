#include "game.h"

bool g_bExit = false;
bool g_bBackMode = false;

IM2SceneNode* g_Node = NULL;
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
	const SMapRecord* mapRecord = g_Engine->getWowDatabase()->getMap(1);

	g_Engine->getEngineSetting()->setViewDistance(EL_FAIR);

	c8 mapname[MAX_PATH];
	sprintf_s(mapname, MAX_PATH, "World\\Maps\\%s\\%s.wdt", mapRecord->name, mapRecord->name);
	fileWDT = g_Engine->getResourceLoader()->loadWDT(mapname, mapRecord->id);
	if (!fileWDT)
		return;

	IWDTSceneNode* node = g_Engine->getSceneManager()->addWDTSceneNode(fileWDT, NULL);
	matrix4 mat;
	mat.setScale(g_Engine->getSceneRenderServices()->SCENE_SCALE);
	node->setRelativeTransformation(mat);
	node->update();

	node->setCurrentTile(29, 40);
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	cam->setPosition(node->getCenter() + vector3df(0, 20.0f, 0));
	cam->recalculateAll();
	
	g_Engine->getSceneManager()->addSkySceneNode(fileWDT->getMapEnvironment());

	/*
	STile* tile = fileWDT->getTile(29,40);
	_ASSERT(tile);
	IMapTileSceneNode* node = g_Engine->getSceneManager()->addMapTileSceneNode(fileWDT, tile, NULL);
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	cam->Position = node->getCenter();
	cam->recalculateAll();
	*/
}

void destroyScene()
{
	if(fileWDT)
		fileWDT->drop();
}

void idleTick()
{

}


