#pragma once

#include "uiButton.h"
#include "uiTexture.h"

class uiCheckButton : public uiButton
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiCheckButton);

public:
	uiCheckButton();

public:
	uiTexture CheckedTexture;
	uiTexture DisabledCheckedTexture;	
	bool Checked;

	u8 Padding[3];
};

//parser
class uiParser_CheckButton
{
	DECLARE_INSTANCE(uiParser_CheckButton)

	typedef uiParser_Button	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiCheckButton& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiCheckButton& control);

	bool loadChild(const pugi::xml_node& node, uiCheckButton& control);
};