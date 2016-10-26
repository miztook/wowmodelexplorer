#pragma once

#include "uiFrame.h"
#include "SColor.h"

class uiModel : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiModel);

public:
	uiModel() {}

public:
	SColorf	FogColor;
	string256	File;
	f32	Scale;
	f32	FogNear;
	f32	FogFar;
};

//parser
class uiParser_Model
{
	DECLARE_INSTANCE(uiParser_Model)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiModel& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiModel& control);
};