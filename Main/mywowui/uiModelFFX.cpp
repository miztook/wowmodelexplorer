#include "stdafx.h"
#include "uiModelFFX.h"
#include "mywowui.h"

bool uiParser_ModelFFX::loadXml( const pugi::xml_node& node, uiModelFFX& control )
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

bool uiParser_ModelFFX::loadProperties( const pugi::xml_node& node, uiModelFFX& control )
{
	return true;
}

