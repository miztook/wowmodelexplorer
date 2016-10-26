#pragma once

#include "uiPlayerModel.h"

class uiTabardModel : public uiPlayerModel
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiTabardModel);

public:
	uiTabardModel() {}

};

//parser
class uiParser_TabardModel
{
	DECLARE_INSTANCE(uiParser_TabardModel)

	typedef uiParser_PlayerModel	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiTabardModel& control);
private:
	bool loadProperties(const pugi::xml_node& node, uiTabardModel& control);
};