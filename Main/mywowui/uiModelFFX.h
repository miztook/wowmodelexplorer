#pragma once

#include "uiModel.h"

class uiModelFFX : public uiModel
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiModelFFX);

public:
	uiModelFFX() {}

};

//parser
class uiParser_ModelFFX
{
	DECLARE_INSTANCE(uiParser_ModelFFX)

	typedef uiParser_Model	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiModelFFX& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiModelFFX& control);
};