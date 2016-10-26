#pragma once

#include "uiPlayerModel.h"

class uiDressUpModel : public uiPlayerModel
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiDressUpModel);

public:
	uiDressUpModel() {}
};

//parser
class uiParser_DressUpModel
{
	DECLARE_INSTANCE(uiParser_DressUpModel)

	typedef uiParser_PlayerModel BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiDressUpModel& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiDressUpModel& control);
};