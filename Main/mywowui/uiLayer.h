#pragma once

#include "core.h"
#include "uibase.h"
#include "uiInheritableObject.h"
#include "uiStructs.h"

#include <list>

class uiLayeredLayoutFrame;

class uiLayer  : public uiInheritableObject
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiLayer);

public:
	uiLayer();

public:
	E_DRAW_LAYER	Level;
	std::list<uiLayeredLayoutFrame*>		Items;
};

//parser
class uiParser_Layer
{
	DECLARE_INSTANCE(uiParser_Layer)

	typedef uiParser_InheritableObject	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiLayer& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiLayer& control);
	bool loadChild(const pugi::xml_node& node, uiLayer& control);
};