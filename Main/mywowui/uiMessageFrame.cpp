#include "stdafx.h"
#include "uiMessageFrame.h"
#include "mywowui.h"

bool uiParser_MessageFrame::loadXml( const pugi::xml_node& node, uiMessageFrame& control )
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

	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
	{
		if (!loadChild(child, control))
		{
			_ASSERT(false);
			return false;
		}
	}

	return true;
}

bool uiParser_MessageFrame::loadProperties( const pugi::xml_node& node, uiMessageFrame& control )
{
	return true;
}

bool uiParser_MessageFrame::loadChild( const pugi::xml_node& node, uiMessageFrame& control )
{
	return true;
}
