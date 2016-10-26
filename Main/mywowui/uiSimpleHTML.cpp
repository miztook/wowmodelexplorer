#include "stdafx.h"
#include "uiSimpleHTML.h"
#include "mywowui.h"

bool uiParser_SimpleHTML::loadXml( const pugi::xml_node& node, uiSimpleHTML& control )
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

bool uiParser_SimpleHTML::loadProperties( const pugi::xml_node& node, uiSimpleHTML& control )
{
	return true;
}
