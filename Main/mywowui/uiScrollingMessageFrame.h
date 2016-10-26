#pragma once

#include "uiMessageFrame.h"

class uiScrollingMessageFrame : public uiMessageFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiScrollingMessageFrame);

public:
	uiScrollingMessageFrame() {}

public:
	s32	MaxLines;
};

//parser
class uiParser_ScrollingMessageFrame
{
	DECLARE_INSTANCE(uiParser_ScrollingMessageFrame)

	typedef uiParser_MessageFrame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiScrollingMessageFrame& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiScrollingMessageFrame& control);
};