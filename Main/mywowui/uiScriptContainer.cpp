#include "stdafx.h"
#include "uiScriptContainer.h"
#include "mywowui.h"

bool uiParser_ScriptContainer::loadXml( const pugi::xml_node& node, uiScriptContainer& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}

	return true;
}

bool uiParser_ScriptContainer::loadProperties( const pugi::xml_node& node, uiScriptContainer& control )
{
	return true;
}
