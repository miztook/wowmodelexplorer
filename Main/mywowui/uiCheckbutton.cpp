#include "stdafx.h"
#include "uiCheckButton.h"
#include "mywowui.h"

uiCheckButton::uiCheckButton()
{
	Checked = false;
}

//parser

bool uiParser_CheckButton::loadXml( const pugi::xml_node& node, uiCheckButton& control )
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

bool uiParser_CheckButton::loadProperties( const pugi::xml_node& node, uiCheckButton& control )
{
	control.Checked = node.attribute("checked").as_bool();	


	return true;
}

bool uiParser_CheckButton::loadChild( const pugi::xml_node& node, uiCheckButton& control )
{
	string64 name = node.name();
	if(name == "CheckedTexture")
	{
		uiParser_Texture::getInstance().loadXml(node, control.CheckedTexture);
	}
	else if (name == "DisabledCheckedTexture")
	{
		uiParser_Texture::getInstance().loadXml(node, control.DisabledCheckedTexture);
	}

	return true;
}

