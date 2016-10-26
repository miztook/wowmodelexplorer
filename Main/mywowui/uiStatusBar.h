#pragma once

#include "uiFrame.h"

class uiTexture;

class uiStatusBar : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiStatusBar);

public:
	uiStatusBar() {}

public:
	E_DRAW_LAYER	DrawLayer;
	f32	MinValue;
	f32	MaxValue;
	f32	DefaultValue;
	E_ORIENTATION	Orientation;
	uiTexture*		BarTexture;
	SColorf	BarColor;
};

//parser
class uiParser_StatusBar
{
	DECLARE_INSTANCE(uiParser_StatusBar)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiStatusBar& control);
private:
	bool loadProperties(const pugi::xml_node& node, uiStatusBar& control);
};