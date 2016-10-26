#include "stdafx.h"
#include "uiScrollFrame.h"
#include "mywowui.h"

bool uiParser_ScrollFrame::loadXml( const pugi::xml_node& node, uiScrollFrame& control )
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

bool uiParser_ScrollFrame::loadProperties( const pugi::xml_node& node, uiScrollFrame& control )
{
	return true;
}
