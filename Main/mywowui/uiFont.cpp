#include "stdafx.h"
#include "uiFont.h"

//parser

bool uiParser_Font::loadXml( const pugi::xml_node& node, uiFont& control )
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

bool uiParser_Font::loadProperties( const pugi::xml_node& node, uiFont& control )
{
	control.Font = node.attribute("font").as_string();
	control.Spacing = node.attribute("spacing").as_float();
	control.Outline = E_OUTLINE_Parse(node.attribute("outline").as_string());
	return true;
}
