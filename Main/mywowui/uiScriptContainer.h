#pragma once

#include "core.h"
#include "uibase.h"

#include <map>

class uiScriptContainer
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiScriptContainer);

public:

};

//parser
class uiParser_ScriptContainer
{
	DECLARE_INSTANCE(uiParser_ScriptContainer)

public:
	bool loadXml(const pugi::xml_node& node, uiScriptContainer& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiScriptContainer& control);
};