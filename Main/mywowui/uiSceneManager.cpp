#include "stdafx.h"
#include "uiSceneManager.h"
#include "mywowui.h"

uiSceneManager::uiSceneManager()
{

}

uiSceneManager::~uiSceneManager()
{
	removeAllUi();
}

uiUi* uiSceneManager::addUi( const c8* filename )
{
	uiUi* ui = getUi(filename);
	if (ui)
		return ui;

	//load
	ui = new uiUi;
	if (g_UISystem->getUiLoader()->loadXml(filename, *ui))
	{
		UiMap.insert(std::pair<string256, uiUi*>(filename, ui));
		return ui;
	}

	g_Engine->getFileSystem()->writeLog(ELOG_UI, "load xml ui failed! filename: %s", filename);

	delete ui;
	return NULL;
}

bool uiSceneManager::removeUi( uiUi* ui )
{
	for (T_UiMap::iterator itr = UiMap.begin(); itr != UiMap.end(); ++itr)
	{
		if (itr->second == ui)
		{
			delete itr->second;
			UiMap.erase(itr);
			return true;
		}
	}
	return false;
}

void uiSceneManager::removeAllUi()
{
	for (T_UiMap::iterator itr = UiMap.begin(); itr != UiMap.end(); ++itr)
	{
		delete itr->second;
	}
	UiMap.clear();
}

uiUi* uiSceneManager::getUi( const c8* filename )
{
	T_UiMap::iterator itr = UiMap.find(filename);
	if (itr != UiMap.end())
	{
		return itr->second;
	}

	return NULL;
}
