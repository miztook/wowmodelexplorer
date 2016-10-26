#pragma once

#include "uibase.h"
#include "uiStructs.h"

class uiParser_SScriptBlock
{
	DECLARE_INSTANCE(uiParser_SScriptBlock)

public:
	bool loadXml(const pugi::xml_node& node, SScriptBlock& control);

private:
	bool loadProperties(const pugi::xml_node& node, SScriptBlock& control);
};

class uiParser_SInclude
{
	DECLARE_INSTANCE(uiParser_SInclude)

public:
	bool loadXml(const pugi::xml_node& node, SInclude& control);

private:
	bool loadProperties(const pugi::xml_node& node, SInclude& control);
};

class uiParser_SAbsValue
{
	DECLARE_INSTANCE(uiParser_SAbsValue)

public:
	bool loadXml(const pugi::xml_node& node, SAbsValue& control);

private:
	bool loadProperties(const pugi::xml_node& node, SAbsValue& control);
};

class uiParser_SAnchor
{
	DECLARE_INSTANCE(uiParser_SAnchor)

public:
	bool loadXml(const pugi::xml_node& node, SAnchor& control);

private:
	bool loadProperties(const pugi::xml_node& node, SAnchor& control);
};

class uiParser_SInset
{
	DECLARE_INSTANCE(uiParser_SInset)

public:
	bool loadXml(const pugi::xml_node& node, SInset& control);

private:
	bool loadProperties(const pugi::xml_node& node, SInset& control);
};

class uiParser_STexCoords
{
	DECLARE_INSTANCE(uiParser_STexCoords)

public:
	bool loadXml(const pugi::xml_node& node, STexCoords& control);

private:
	bool loadProperties(const pugi::xml_node& node, STexCoords& control);
};

class uiParser_SBackDrop
{
	DECLARE_INSTANCE(uiParser_SBackDrop)

public:
	bool loadXml(const pugi::xml_node& node, SBackDrop& control);

private:
	bool loadProperties(const pugi::xml_node& node, SBackDrop& control);
};

class uiParser_SResizeBounds
{
	DECLARE_INSTANCE(uiParser_SResizeBounds)

public:
	bool loadXml(const pugi::xml_node& node, SResizeBounds& control);

private:
	bool loadProperties(const pugi::xml_node& node, SResizeBounds& control);
};

class uiParser_SButtonStyle
{
	DECLARE_INSTANCE(uiParser_SButtonStyle)

public:
	bool loadXml(const pugi::xml_node& node, SButtonStyle& control);

private:
	bool loadProperties(const pugi::xml_node& node, SButtonStyle& control);
};

class uiParser_SAbsDimension
{
	DECLARE_INSTANCE(uiParser_SAbsDimension)

public:
	bool loadXml(const pugi::xml_node& node, SAbsDimension& control);

private:
	bool loadProperties(const pugi::xml_node& node, SAbsDimension& control);
};

class uiParser_SRelDimension
{
	DECLARE_INSTANCE(uiParser_SRelDimension)

public:
	bool loadXml(const pugi::xml_node& node, SRelDimension& control);

private:
	bool loadProperties(const pugi::xml_node& node, SRelDimension& control);
};

class uiParser_SDimension
{
	DECLARE_INSTANCE(uiParser_SDimension)

public:
	bool loadXml(const pugi::xml_node& node, SDimension& control);

private:
	bool loadProperties(const pugi::xml_node& node, SDimension& control);
};

class uiParser_SFontHeight
{
	DECLARE_INSTANCE(uiParser_SFontHeight)

public:
	bool loadXml(const pugi::xml_node& node, SFontHeight& control);

private:
	bool loadProperties(const pugi::xml_node& node, SFontHeight& control);
};

class uiParser_SShadow
{
	DECLARE_INSTANCE(uiParser_SShadow)

public:
	bool loadXml(const pugi::xml_node& node, SShadow& control);

private:
	bool loadProperties(const pugi::xml_node& node, SShadow& control);
};
