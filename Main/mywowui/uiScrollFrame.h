#pragma once

#include "uiFrame.h"

class uiScrollFrame : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiScrollFrame);

public:
	uiScrollFrame() {}

public:
	std::list<uiFrame*>		ScrollChildren;
};

//parser
class uiParser_ScrollFrame
{
	DECLARE_INSTANCE(uiParser_ScrollFrame)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiScrollFrame& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiScrollFrame& control);
};