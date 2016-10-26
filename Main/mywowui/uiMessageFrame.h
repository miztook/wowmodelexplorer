#pragma once

#include "uiFrame.h"

class uiMessageFrame : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiMessageFrame);

public:
	uiMessageFrame() {}

public:
	string64		Font;
	f32	FadeDuration;
	f32	DisplayDuration;
	E_INSERT_MODE	InsertMode;
	bool	Fade;
};

//parser
class uiParser_MessageFrame
{
	DECLARE_INSTANCE(uiParser_MessageFrame)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiMessageFrame& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiMessageFrame& control);
	bool loadChild(const pugi::xml_node& node, uiMessageFrame& control);
};