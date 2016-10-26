#pragma once

#include "core.h"
#include "pugixml.hpp"
#include "uibase.h"

class uiUi;

class uiLoader
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiLoader);

public:
	uiLoader();
	~uiLoader();

public:
	bool loadXml(const c8* filename, uiUi& ui);

private:
	bool loadXml(const pugi::xml_node& node, uiUi& ui);
};