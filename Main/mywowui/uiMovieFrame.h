#pragma once

#include "uiFrame.h"

class uiMovieFrame : public uiFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiMovieFrame);

public:
	uiMovieFrame() {}

};

//parser
class uiParser_MovieFrame
{
	DECLARE_INSTANCE(uiParser_MovieFrame)

	typedef uiParser_Frame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiMovieFrame& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiMovieFrame& control);
};