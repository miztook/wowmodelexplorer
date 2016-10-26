#pragma once

#include "core.h"
#include "uibase.h"
#include "uiInheritableObject.h"
#include "pugixml.hpp"

class uiFont : public uiInheritableObject
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiFont);

public:
	uiFont(): Spacing(0), Outline(Outline_None), MonoChrome(false) {}

public:
	string64	Font;
	f32	Spacing;
	E_OUTLINE		Outline;
	bool MonoChrome;

	u8	Padding[3];
};

//parser
class uiParser_Font
{
	DECLARE_INSTANCE(uiParser_Font)

	typedef uiParser_InheritableObject	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiFont& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiFont& control);
};