#include "stdafx.h"
#include "uiSlider.h"
#include "mywowui.h"

bool uiParser_Slider::loadXml( const pugi::xml_node& node, uiSlider& control )
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

bool uiParser_Slider::loadProperties( const pugi::xml_node& node, uiSlider& control )
{
	return true;
}