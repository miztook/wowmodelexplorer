#pragma once

#include "uiFrame.h"

class uiMinimap : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiMinimap);

public:
	uiMinimap() {}

public:

};

//parser
class uiParser_Minimap
{
	DECLARE_INSTANCE(uiParser_Minimap)

	typedef uiParser_Frame		BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiMinimap& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiMinimap& control);
};