#pragma once

#include "uiFrame.h"

class uiTexture;

class uiSlider : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiSlider);

public:
	uiSlider() {}

public:
	E_DRAW_LAYER	DrawLayer;
	f32	MinValue;
	f32	MaxValue;
	f32	DefaultValue;
	f32	ValueStep;
	E_ORIENTATION	Orientation;
	uiTexture*		ThumbTexture;

};

//parser
class uiParser_Slider
{
	DECLARE_INSTANCE(uiParser_Slider)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiSlider& control);
private:
	bool loadProperties(const pugi::xml_node& node, uiSlider& control);
};