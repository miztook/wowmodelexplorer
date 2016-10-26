#pragma once

#include "uiLayoutFrame.h"

class uiLayeredLayoutFrame : public uiLayoutFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiLayeredLayoutFrame);

public:
	explicit uiLayeredLayoutFrame(E_DRAW_LAYER drawLayer)
		: DrawLayer(drawLayer) {}

protected:
	E_DRAW_LAYER	DrawLayer;
};

//parser
class uiParser_LayeredLayoutFrame
{
	DECLARE_INSTANCE(uiParser_LayeredLayoutFrame)

	typedef uiParser_LayoutFrame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiLayeredLayoutFrame& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiLayeredLayoutFrame& control);
	bool loadChild(const pugi::xml_node& node, uiLayeredLayoutFrame& control);
};