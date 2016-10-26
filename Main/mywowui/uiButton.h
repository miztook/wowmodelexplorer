#pragma once

#include "uiFrame.h"
#include "uiStructs.h"
#include "SColor.h"
#include "uiTexture.h"
#include "uiFontString.h"

class uiButton : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiButton);

public:
	uiButton();

	virtual ~uiButton();

public:
	//attribute
	string256	Text;

	//element
	uiTexture		NormalTexture;
	uiTexture		PushedTexture;
	uiTexture		DisabledTexture;
	uiTexture		HighlightTexture;
	uiFontString		ButtonText;
	SButtonStyle		HilightFont;
	SButtonStyle		DisabledFont;
	SColorf		NormalColor;
	SColorf		HilightColor;
	SColorf		DisabledColor;
	SAbsDimension	PushedTextOffset;
};

//parser
class uiParser_Button
{
	DECLARE_INSTANCE(uiParser_Button)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiButton& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiButton& control);

	bool loadChild(const pugi::xml_node& node, uiButton& control);
};