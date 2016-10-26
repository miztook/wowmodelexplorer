#pragma once

#include "uiFrame.h"

class uiGameTooltip : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiGameTooltip);

public:
	uiGameTooltip() {}

};

//parser
class uiParser_GameTooltip
{
	DECLARE_INSTANCE(uiParser_GameTooltip)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiGameTooltip& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiGameTooltip& control);
};