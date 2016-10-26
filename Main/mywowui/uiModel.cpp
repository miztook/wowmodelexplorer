#include "stdafx.h"
#include "uiModel.h"
#include "mywowui.h"

bool uiParser_Model::loadXml( const pugi::xml_node& node, uiModel& control )
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

bool uiParser_Model::loadProperties( const pugi::xml_node& node, uiModel& control )
{
	return true;
}

