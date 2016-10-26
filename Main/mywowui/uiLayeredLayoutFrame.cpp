#include "stdafx.h"
#include "uiLayeredLayoutFrame.h"
#include "mywowui.h"

bool uiParser_LayeredLayoutFrame::loadXml( const pugi::xml_node& node, uiLayeredLayoutFrame& control )
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

bool uiParser_LayeredLayoutFrame::loadProperties( const pugi::xml_node& node, uiLayeredLayoutFrame& control )
{
	return true;
}

bool uiParser_LayeredLayoutFrame::loadChild( const pugi::xml_node& node, uiLayeredLayoutFrame& control )
{
	return true;
}
