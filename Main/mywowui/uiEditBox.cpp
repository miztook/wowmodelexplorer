#include "stdafx.h"
#include "uiEditBox.h"
#include "mywowui.h"

bool uiParser_EditBox::loadXml( const pugi::xml_node& node, uiEditBox& control )
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

bool uiParser_EditBox::loadProperties( const pugi::xml_node& node, uiEditBox& control )
{
	return true;
}

bool uiParser_EditBox::loadChild( const pugi::xml_node& node, uiEditBox& control )
{
	return true;
}
