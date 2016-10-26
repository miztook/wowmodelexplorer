#pragma once

#include "uiFrame.h"

class uiTaxiRouteFrame : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiTaxiRouteFrame);

public:
	uiTaxiRouteFrame() {}

};

//parser
class uiParser_TaxiRouteFrame
{
	DECLARE_INSTANCE(uiParser_TaxiRouteFrame)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiTaxiRouteFrame& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiTaxiRouteFrame& control);
};