#include "stdafx.h"
#include "uiPlayerModel.h"
#include "mywowui.h"

bool uiParser_PlayerModel::loadXml( const pugi::xml_node& node, uiPlayerModel& control )
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

bool uiParser_PlayerModel::loadProperties( const pugi::xml_node& node, uiPlayerModel& control )
{
	return true;
}
