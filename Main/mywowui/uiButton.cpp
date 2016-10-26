#include "stdafx.h"
#include "uiButton.h"
#include "mywowui.h"

uiButton::uiButton()
{

}

uiButton::~uiButton()
{

}

//parser
bool uiParser_Button::loadXml( const pugi::xml_node& node, uiButton& control )
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

bool uiParser_Button::loadProperties( const pugi::xml_node& node, uiButton& control )
{
	control.Text = node.attribute("text").as_string();

	return true;
}

bool uiParser_Button::loadChild( const pugi::xml_node& node, uiButton& control )
{
#define START_LOAD_XML(controlname, parser, member) \
	if(name == controlname)		\
	{															\
		parser::getInstance().loadXml(node, control.##member);		\
	}

#define LOAD_XML(controlname, parser, member) \
	else if(name == controlname)		\
	{															\
		parser::getInstance().loadXml(node, control.##member);		\
	}

	//start 
	string64 name = node.name();

	START_LOAD_XML("NormalTexture", uiParser_Texture, NormalTexture)
	LOAD_XML("PushedTexture",	uiParser_Texture, PushedTexture)
	LOAD_XML("DisabledTexture",	uiParser_Texture, DisabledTexture)
	LOAD_XML("HighlightTexture",	uiParser_Texture, HighlightTexture)
	LOAD_XML("ButtonText",	uiParser_FontString, ButtonText)
	LOAD_XML("HilightFont",	uiParser_SButtonStyle, HilightFont)
	LOAD_XML("DisabledFont",	uiParser_SButtonStyle, DisabledFont)
	
	return true;
}
