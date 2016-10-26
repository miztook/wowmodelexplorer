#include "stdafx.h"
#include "uiWorldFrame.h"
#include "mywowui.h"

bool uiParser_WorldFrame::loadXml( const pugi::xml_node& node, uiWorldFrame& control )
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

bool uiParser_WorldFrame::loadProperties( const pugi::xml_node& node, uiWorldFrame& control )
{
	return true;
}