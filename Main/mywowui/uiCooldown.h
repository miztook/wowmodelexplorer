#pragma once

#include "uiFrame.h"

class uiCooldown : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiCooldown);

public:
	uiCooldown() {}

public:
	bool Reverse;

	u8	Padding[3];
};

//parser
class uiParser_Cooldown
{
	DECLARE_INSTANCE(uiParser_Cooldown)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiCooldown& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiCooldown& control);

	bool loadChild(const pugi::xml_node& node, uiCooldown& control);
};