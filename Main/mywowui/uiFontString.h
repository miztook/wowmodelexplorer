#pragma once

#include "uiLayeredLayoutFrame.h"

class uiFontString : public uiLayeredLayoutFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiFontString);

public:
	uiFontString() : uiLayeredLayoutFrame(EDL_OVERLAY) {}

public:
	string256		Font;
	string256		Text;
	int		Bytes;
	f32		Spacing;
	E_OUTLINE		Outline;
	E_JUSTIFY_VERTICAL	JustifyVertical;
	u32		MaxLines;
	SFontHeight		FontHeight;
	SColorf		Color;
	bool		Monochrome;
	bool		NonSpcaeWrap;

	u8	Padding[2];
};

//parser
class uiParser_FontString
{
	DECLARE_INSTANCE(uiParser_FontString)

	typedef uiParser_LayeredLayoutFrame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiFontString& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiFontString& control);
};