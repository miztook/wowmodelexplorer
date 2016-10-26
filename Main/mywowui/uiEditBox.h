#pragma once

#include "uiFrame.h"
#include "pugixml.hpp"

class uiFontString;

class uiEditBox : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiEditBox);

public:
	uiEditBox() {}

public:
	string64		Font;
	uiFontString*	FontString;
	s32		HistoryLines;
	s32		Letters;
	f32		BlinkSpeed;
	bool	Numeric;
	bool	Password;
	bool	MultiLine;
	bool	AutoFocus;
	bool	IgnoreArrows;

	u8	Padding[3];
};

//parser
class uiParser_EditBox
{
	DECLARE_INSTANCE(uiParser_EditBox)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiEditBox& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiEditBox& control);

	bool loadChild(const pugi::xml_node& node, uiEditBox& control);
};