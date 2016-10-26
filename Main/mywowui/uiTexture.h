#pragma once

#include "uiLayeredLayoutFrame.h"

class uiTexture : public uiLayeredLayoutFrame
{

public:
	uiTexture();

public:
	string256		File;
	E_ALPHA_MODE	AlphaMode;
	STexCoords		TexCoords;
	SColor		Color;
};

//parser
class uiParser_Texture
{
	DECLARE_INSTANCE(uiParser_Texture)

	typedef uiParser_LayeredLayoutFrame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiTexture& control);
private:
	bool loadProperties(const pugi::xml_node& node, uiTexture& control);
	bool loadChild(const pugi::xml_node& node, uiTexture& control);
};