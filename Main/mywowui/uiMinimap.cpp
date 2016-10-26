#include "stdafx.h"
#include "uiMinimap.h"
#include "mywowui.h"

bool uiParser_Minimap::loadXml( const pugi::xml_node& node, uiMinimap& control )
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

bool uiParser_Minimap::loadProperties( const pugi::xml_node& node, uiMinimap& control )
{
	return true;
}

