#include "stdafx.h"
#include "uiTabardModel.h"
#include "mywowui.h"

bool uiParser_TabardModel::loadXml( const pugi::xml_node& node, uiTabardModel& control )
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

bool uiParser_TabardModel::loadProperties( const pugi::xml_node& node, uiTabardModel& control )
{
	return true;
}