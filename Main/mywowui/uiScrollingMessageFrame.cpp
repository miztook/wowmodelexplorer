#include "stdafx.h"
#include "uiScrollingMessageFrame.h"
#include "mywowui.h"

bool uiParser_ScrollingMessageFrame::loadXml( const pugi::xml_node& node, uiScrollingMessageFrame& control )
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

bool uiParser_ScrollingMessageFrame::loadProperties( const pugi::xml_node& node, uiScrollingMessageFrame& control )
{
	return true;
}
