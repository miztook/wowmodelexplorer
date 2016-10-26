#pragma once

#include "uiInheritableObject.h"
#include "uiStructs.h"
#include <vector>

class uiLayoutFrame : public uiInheritableObject
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiLayoutFrame);

public:
	uiLayoutFrame() {}

public:
	virtual void render() {}

public:
	dimension2du	SizeDimension;
	std::vector<SAnchor>		Anchors;
	bool SetAllPoints;
	bool Hidden;

	u8	Padding[2];

private:
	uiLayoutFrame*	ParentFrame;
	bool		Locked;
	bool		LayoutHidden;
	bool		IsInVirtualState;

	u8	Padding2[1];
};

//parser
class uiParser_LayoutFrame
{
	DECLARE_INSTANCE(uiParser_LayoutFrame)

	typedef uiParser_InheritableObject	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiLayoutFrame& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiLayoutFrame& control);
	bool loadChild(const pugi::xml_node& node, uiLayoutFrame& control);
};