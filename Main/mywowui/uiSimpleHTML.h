#pragma once

#include "uiFrame.h"

class uiFontString;

class uiSimpleHTML : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiSimpleHTML);

public:
	uiSimpleHTML() {}

public:
	uiFontString*	FontString;
	uiFontString*	FontStringHeader1;
	uiFontString*	FontStringHeader2;
	uiFontString*	FontStringHeader3;
	string64		Font;
	string256		File;
	string256		HyperLinkFormat;
};

//parser
class uiParser_SimpleHTML
{
	DECLARE_INSTANCE(uiParser_SimpleHTML)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiSimpleHTML& control);
private:
	bool loadProperties(const pugi::xml_node& node, uiSimpleHTML& control);
};