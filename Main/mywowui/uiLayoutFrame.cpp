#include "stdafx.h"
#include "uiLayoutFrame.h"

//parser

bool uiParser_LayoutFrame::loadXml( const pugi::xml_node& node, uiLayoutFrame& control )
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

bool uiParser_LayoutFrame::loadProperties( const pugi::xml_node& node, uiLayoutFrame& control )
{
	return true;
}

bool uiParser_LayoutFrame::loadChild( const pugi::xml_node& node, uiLayoutFrame& control )
{
	return true;
}
