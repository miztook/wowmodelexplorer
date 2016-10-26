#include "stdafx.h"
#include "uiParser_Struct.h"
#include "mywowui.h"

bool uiParser_SScriptBlock::loadXml( const pugi::xml_node& node, SScriptBlock& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SScriptBlock::loadProperties( const pugi::xml_node& node, SScriptBlock& control )
{
	control.ScriptBlock = NULL;
	control.ScriptLength = 0;
	control.File = node.attribute("file").as_string();

	return true;
}

bool uiParser_SInclude::loadXml( const pugi::xml_node& node, SInclude& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SInclude::loadProperties( const pugi::xml_node& node, SInclude& control )
{
	control.File = node.attribute("file").as_string();

	return true;
}

bool uiParser_SAbsValue::loadXml( const pugi::xml_node& node, SAbsValue& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SAbsValue::loadProperties( const pugi::xml_node& node, SAbsValue& control )
{
	control.Value = node.attribute("val").as_int();

	return true;
}

bool uiParser_SAnchor::loadXml( const pugi::xml_node& node, SAnchor& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SAnchor::loadProperties( const pugi::xml_node& node, SAnchor& control )
{
	control.Point = (E_FRAME_POINT)node.attribute("point").as_int();
	control.RelativePoint = (E_FRAME_POINT)node.attribute("relativePoint").as_int();
	control.RelativeTo = node.attribute("relativeTo").as_string();

	return true;
}

bool uiParser_SInset::loadXml( const pugi::xml_node& node, SInset& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SInset::loadProperties( const pugi::xml_node& node, SInset& control )
{
	control.Left = node.attribute("left").as_int();
	control.Right = node.attribute("right").as_int();
	control.Top = node.attribute("top").as_int();
	control.Bottom = node.attribute("bottom").as_int();

	return true;
}

bool uiParser_STexCoords::loadXml( const pugi::xml_node& node, STexCoords& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_STexCoords::loadProperties( const pugi::xml_node& node, STexCoords& control )
{
	return true;
}

bool uiParser_SBackDrop::loadXml( const pugi::xml_node& node, SBackDrop& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SBackDrop::loadProperties( const pugi::xml_node& node, SBackDrop& control )
{
	return true;
}

bool uiParser_SResizeBounds::loadXml( const pugi::xml_node& node, SResizeBounds& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SResizeBounds::loadProperties( const pugi::xml_node& node, SResizeBounds& control )
{
	return true;
}

bool uiParser_SButtonStyle::loadXml( const pugi::xml_node& node, SButtonStyle& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}
	return true;
}

bool uiParser_SButtonStyle::loadProperties( const pugi::xml_node& node, SButtonStyle& control )
{
	return true;
}
