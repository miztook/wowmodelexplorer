#include "stdafx.h"
#include "uiTaxiRouteFrame.h"
#include "mywowui.h"

bool uiParser_TaxiRouteFrame::loadXml( const pugi::xml_node& node, uiTaxiRouteFrame& control )
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

bool uiParser_TaxiRouteFrame::loadProperties( const pugi::xml_node& node, uiTaxiRouteFrame& control )
{
	return true;
}