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

	{
		IFileM2* m2PandarenMale = g_Engine->getResourceLoader()->loadM2("Character\\PANDAREN\\FEMALE\\PandarenFemale.m2");
		IM2SceneNode*  pandarenMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2PandarenMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		pandarenMaleNode->getM2Appearance()->loadStartOutfit(446, false, true);
		pandarenMaleNode->getM2Appearance()->takeItem(32375, true);
		pandarenMaleNode->updateCharacter();
		pandarenMaleNode->getM2Move()->setPos(vector3df(0, 0, -1));
		pandarenMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		pandarenMaleNode->getM2FSM()->changeState(EMS_STAND);
	}
	
	//human
	{
		IFileM2* m2HumanMale= g_Engine->getResourceLoader()->loadM2("Character\\HUMAN\\FEMALE\\HumanFemale.m2");
		IM2SceneNode*  m2HumanMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2HumanMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2HumanMaleNode->getM2Appearance()->loadStartOutfit(1, false, true);
		m2HumanMaleNode->updateCharacter();
		m2HumanMaleNode->getM2Move()->setPos(vector3df(-1.5f, 0, -1));
		m2HumanMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		m2HumanMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//nightelf
	{
		IFileM2* m2NightElfMale= g_Engine->getResourceLoader()->loadM2("Character\\NIGHTELF\\FEMALE\\NightElfFemale.m2");
		IM2SceneNode*  m2NightElfMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2NightElfMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2NightElfMaleNode->getM2Appearance()->loadStartOutfit(4, false, true);
		m2NightElfMaleNode->updateCharacter();
		m2NightElfMaleNode->getM2Move()->setPos(vector3df(1.5f, 0, -1));
		m2NightElfMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		m2NightElfMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//dwarf
	{
		IFileM2* m2DwarfMale= g_Engine->getResourceLoader()->loadM2("Character\\DWARF\\FEMALE\\DwarfFemale.m2");
		IM2SceneNode*  m2DwarfMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2DwarfMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2DwarfMaleNode->getM2Appearance()->loadStartOutfit(3, false, true);
		m2DwarfMaleNode->updateCharacter();
		m2DwarfMaleNode->getM2Move()->setPos(vector3df(-3.0f, 0, -1));
		m2DwarfMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		m2DwarfMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//gnome
	{
		IFileM2* m2GnomeMale= g_Engine->getResourceLoader()->loadM2("Character\\GNOME\\FEMALE\\GnomeFemale.m2");
		IM2SceneNode*  m2GnomeMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2GnomeMale, NULL);	
		m2GnomeMaleNode->getM2Instance()->CharacterInfo->HairStyle = 3;	
		m2GnomeMaleNode->getM2Appearance()->loadStartOutfit(291, false, true);
		m2GnomeMaleNode->updateCharacter();
		m2GnomeMaleNode->getM2Move()->setPos(vector3df(3.0f, 0, -1));
		m2GnomeMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		m2GnomeMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//draenei
	{
		IFileM2* m2DraeneiMale= g_Engine->getResourceLoader()->loadM2("Character\\DRAENEI\\FEMALE\\DraeneiFemale.m2");
		IM2SceneNode*  m2DraeneiMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2DraeneiMale, NULL);	
		m2DraeneiMaleNode->getM2Instance()->CharacterInfo->FacialHair = 1;	
		m2DraeneiMaleNode->getM2Appearance()->loadStartOutfit(331, false, true);
		m2DraeneiMaleNode->updateCharacter();
		m2DraeneiMaleNode->getM2Move()->setPos(vector3df(-4.5f, 0, -1));
		m2DraeneiMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		m2DraeneiMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//worgen
	{
		IFileM2* m2WorgenMale= g_Engine->getResourceLoader()->loadM2("Character\\WORGEN\\FEMALE\\WorgenFemale.m2");
		IM2SceneNode*  m2WorgenMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2WorgenMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2WorgenMaleNode->getM2Appearance()->loadStartOutfit(368, false, true);
		m2WorgenMaleNode->updateCharacter();
		m2WorgenMaleNode->getM2Move()->setPos(vector3df(4.5f, 0, -1));
		m2WorgenMaleNode->getM2Move()->setDir(-vector3df::UnitZ());
		m2WorgenMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//pandaren
	{
		IFileM2* m2PandarenMale = g_Engine->getResourceLoader()->loadM2("Character\\PANDAREN\\FEMALE\\PandarenFemale.m2");
		IM2SceneNode*  pandarenMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2PandarenMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		pandarenMaleNode->getM2Appearance()->loadStartOutfit(446, false, false);
		pandarenMaleNode->updateCharacter();
		pandarenMaleNode->getM2Move()->setPos(vector3df(0, 0, 1));
		pandarenMaleNode->getM2Move()->setDir(vector3df::UnitZ());
		pandarenMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//orc
	{
		IFileM2* m2OrcMale= g_Engine->getResourceLoader()->loadM2("Character\\ORC\\FEMALE\\OrcFemale.m2");
		IM2SceneNode*  m2OrcMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2OrcMale, NULL);	
		m2OrcMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2OrcMaleNode->getM2Appearance()->loadStartOutfit(2, false, true);
		m2OrcMaleNode->updateCharacter();
		m2OrcMaleNode->getM2Move()->setPos(vector3df(-1.5f, 0, 1));
		m2OrcMaleNode->getM2Move()->setDir(vector3df::UnitZ());
		m2OrcMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//tauren
	{
		IFileM2* m2TaurenMale= g_Engine->getResourceLoader()->loadM2("Character\\TAUREN\\FEMALE\\TaurenFemale.m2");
		IM2SceneNode*  m2TaurenMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2TaurenMale, NULL);	
		m2TaurenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 1;	
		m2TaurenMaleNode->getM2Appearance()->loadStartOutfit(27, false, true);
		m2TaurenMaleNode->updateCharacter();
		m2TaurenMaleNode->getM2Move()->setPos(vector3df(1.5f, 0, 1));
		m2TaurenMaleNode->getM2Move()->setDir(vector3df::UnitZ());
		m2TaurenMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//scourge
	{
		IFileM2* m2ScourgeMale= g_Engine->getResourceLoader()->loadM2("Character\\SCOURGE\\FEMALE\\ScourgeFemale.m2");
		IM2SceneNode*  m2ScourgeMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2ScourgeMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2ScourgeMaleNode->getM2Appearance()->loadStartOutfit(34, false, true);
		m2ScourgeMaleNode->updateCharacter();
		m2ScourgeMaleNode->getM2Move()->setPos(vector3df(-3.0f, 0, 1));
		m2ScourgeMaleNode->getM2Move()->setDir(vector3df::UnitZ());
		m2ScourgeMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//troll
	{
		IFileM2* m2TrollMale= g_Engine->getResourceLoader()->loadM2("Character\\TROLL\\FEMALE\\TrollFemale.m2");
		IM2SceneNode*  m2TrollMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2TrollMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2TrollMaleNode->getM2Appearance()->loadStartOutfit(292, false, true);
		m2TrollMaleNode->updateCharacter();
		m2TrollMaleNode->getM2Move()->setPos(vector3df(3.0f, 0, 1));
		m2TrollMaleNode->getM2Move()->setDir(vector3df::UnitZ());
		m2TrollMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//bloodelf
	{
		IFileM2* m2BloodElfMale= g_Engine->getResourceLoader()->loadM2("Character\\BLOODELF\\FEMALE\\BloodElfFemale.m2");
		IM2SceneNode*  m2BloodElfMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2BloodElfMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2BloodElfMaleNode->getM2Appearance()->loadStartOutfit(402, false, true);
		m2BloodElfMaleNode->updateCharacter();
		m2BloodElfMaleNode->getM2Move()->setPos(vector3df(-4.5f, 0, 1));
		m2BloodElfMaleNode->getM2Move()->setDir(vector3df::UnitZ());
		m2BloodElfMaleNode->getM2FSM()->changeState(EMS_STAND);
	}

	//goblin
	{
		IFileM2* m2GoblinMale= g_Engine->getResourceLoader()->loadM2("Character\\GOBLIN\\FEMALE\\GoblinFemale.m2");
		IM2SceneNode*  m2GoblinMaleNode = g_Engine->getSceneManager()->addM2SceneNode(m2GoblinMale, NULL);	
		//pandarenMaleNode->getM2Instance()->CharacterInfo->FacialHair = 2;	
		m2GoblinMaleNode->getM2Appearance()->loadStartOutfit(382, false, true);
		m2GoblinMaleNode->updateCharacter();
		m2GoblinMaleNode->getM2Move()->setPos(vector3df(4.5f, 0, 1));
		m2GoblinMaleNode->getM2Move()->setDir(vector3df::UnitZ());
		m2GoblinMaleNode->getM2FSM()->changeState(EMS_STAND);
	}
}

void destroyScene()
{
	g_Engine->getSceneManager()->removeAllSceneNodes();
}

void idleTick()
{
}
