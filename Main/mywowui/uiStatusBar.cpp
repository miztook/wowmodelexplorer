#include "stdafx.h"
#include "uiStatusBar.h"
#include "mywowui.h"

bool uiParser_StatusBar::loadXml( const pugi::xml_node& node, uiStatusBar& control )
{
	if (!BaseParser::getInstance().loadXml(node, control))
	{
		_ASSERT(false);
		return false;
	}

	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}

	return true;
}

bool uiParser_StatusBar::loadProperties( const pugi::xml_node& node, uiStatusBar& control )
{
	return true;
}