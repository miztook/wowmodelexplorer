#include "stdafx.h"
#include "uiColorSelect.h"

bool uiParser_ColorSelect::loadXml( const pugi::xml_node& node, uiColorSelect& control )
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

bool uiParser_ColorSelect::loadProperties( const pugi::xml_node& node, uiColorSelect& control )
{
	return true;
}

bool uiParser_ColorSelect::loadChild( const pugi::xml_node& node, uiColorSelect& control )
{
	return true;
}
