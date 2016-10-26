#include "stdafx.h"
#include "uiFrame.h"


bool uiParser_Frame::loadXml( const pugi::xml_node& node, uiFrame& control )
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

bool uiParser_Frame::loadProperties( const pugi::xml_node& node, uiFrame& control )
{
	return true;
}

bool uiParser_Frame::loadChild( const pugi::xml_node& node, uiFrame& control )
{
	return true;
}
