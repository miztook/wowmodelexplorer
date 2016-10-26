#include "stdafx.h"
#include "mywowui.h"

UISystem*		g_UISystem = NULL;

void createUISystem()
{
	g_UISystem = new UISystem;	
}

void destroyUISystem()
{
	delete g_UISystem;
}

UISystem::UISystem()
{
	UILoader = new uiLoader;
	UIFrameXmlDirectory = new uiFrameXmlDirectory;
	UISceneManager = new uiSceneManager;
}

UISystem::~UISystem()
{
	delete UISceneManager;
	delete UIFrameXmlDirectory;
	delete UILoader;
}
