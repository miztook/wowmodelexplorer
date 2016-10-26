#include "stdafx.h"
#include "uiLayer.h"
#include "mywowui.h"

uiLayer::uiLayer()
{
	Level = EDL_ARTWORK;
}

bool uiParser_Layer::loadXml( const pugi::xml_node& node, uiLayer& control )
{
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

bool uiParser_Layer::loadProperties( const pugi::xml_node& node, uiLayer& control )
{
	return true;
}

bool uiParser_Layer::loadChild( const pugi::xml_node& node, uiLayer& control )
{
	return true;
}
