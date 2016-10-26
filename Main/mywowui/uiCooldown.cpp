#include "stdafx.h"
#include "uiCooldown.h"
#include "mywowui.h"

bool uiParser_Cooldown::loadXml( const pugi::xml_node& node, uiCooldown& control )
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

bool uiParser_Cooldown::loadProperties( const pugi::xml_node& node, uiCooldown& control )
{
	return true;
}

bool uiParser_Cooldown::loadChild( const pugi::xml_node& node, uiCooldown& control )
{
	return true;
}
