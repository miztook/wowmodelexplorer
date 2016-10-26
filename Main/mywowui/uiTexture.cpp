#include "stdafx.h"
#include "uiTexture.h"

uiTexture::uiTexture()
	: uiLayeredLayoutFrame(EDL_ARTWORK)
{
	AlphaMode = EAM_BLEND;
}

bool uiParser_Texture::loadXml( const pugi::xml_node& node, uiTexture& control )
{
	if (!BaseParser::getInstance().loadXml(node, control))
	{
		_ASSERT(false);
		return false;
	}

	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}

	return true;
}

bool uiParser_Texture::loadProperties( const pugi::xml_node& node, uiTexture& control )
{
	control.File = node.attribute("file").as_string();
	return true;
}
