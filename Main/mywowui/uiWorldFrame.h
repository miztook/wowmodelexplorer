#pragma once

#include "uiFrame.h"

class uiWorldFrame : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiWorldFrame);

public:
	uiWorldFrame() {}

};

//parser
class uiParser_WorldFrame
{
	DECLARE_INSTANCE(uiParser_WorldFrame)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiWorldFrame& control);
private:
	bool loadProperties(const pugi::xml_node& node, uiWorldFrame& control);
};