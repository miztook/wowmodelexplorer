#include "stdafx.h"
#include "uiGameTooltip.h"
#include "mywowui.h"

bool uiParser_GameTooltip::loadXml( const pugi::xml_node& node, uiGameTooltip& control )
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

bool uiParser_GameTooltip::loadProperties( const pugi::xml_node& node, uiGameTooltip& control )
{
	return true;
}


