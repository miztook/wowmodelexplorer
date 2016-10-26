#pragma once

#include "uiModel.h"

class uiPlayerModel : public uiModel
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiPlayerModel);

public:
	uiPlayerModel() {}
};

//parser
class uiParser_PlayerModel
{
	DECLARE_INSTANCE(uiParser_PlayerModel)

	typedef uiParser_Model		BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiPlayerModel& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiPlayerModel& control);
};