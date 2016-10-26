#pragma once

#include "uiFrame.h"
#include "uiTexture.h"

class uiColorSelect : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiColorSelect);

public:
	uiColorSelect() {}

public:
	uiTexture ColorWheelTexture;
	uiTexture ColorWheelThumbTexture;
	uiTexture ColorValueTexture;
	uiTexture ColorValueThumbTexture;
};

//parser
class uiParser_ColorSelect
{
	DECLARE_INSTANCE(uiParser_ColorSelect)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiColorSelect& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiColorSelect& control);

	bool loadChild(const pugi::xml_node& node, uiColorSelect& control);
};