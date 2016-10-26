#include "stdafx.h"
#include "uiFontString.h"
#include "mywowui.h"

bool uiParser_FontString::loadXml( const pugi::xml_node& node, uiFontString& control )
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

bool uiParser_FontString::loadProperties( const pugi::xml_node& node, uiFontString& control )
{
	return true;
}
