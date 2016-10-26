#include "stdafx.h"
#include "uiMovieFrame.h"
#include "mywowui.h"

bool uiParser_MovieFrame::loadXml( const pugi::xml_node& node, uiMovieFrame& control )
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

bool uiParser_MovieFrame::loadProperties( const pugi::xml_node& node, uiMovieFrame& control )
{
	return true;
}
