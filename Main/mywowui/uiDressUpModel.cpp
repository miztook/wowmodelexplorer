#include "stdafx.h"
#include "uiDressUpModel.h"
#include "mywowui.h"

bool uiParser_DressUpModel::loadXml( const pugi::xml_node& node, uiDressUpModel& control )
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

bool uiParser_DressUpModel::loadProperties( const pugi::xml_node& node, uiDressUpModel& control )
{
	return true;
}

