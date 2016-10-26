#pragma once

#include "uiLayoutFrame.h"
#include <list>

class uiLayer;
class uiScriptContainer;

class uiFrame : public uiLayoutFrame
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiFrame);

public:
	uiFrame() {}

public:
	f32		Alpha;
	string64	Parent;
	E_FRAME_STRATA		FrameStrata;	
	uiLayoutFrame*	TitleRegion;
	SResizeBounds		ResizeBounds;
	SBackDrop		Backdrop;
	s32		Id;
	bool		TopLevel;
	bool		Movable;
	bool		Resizable;
	bool		EnableMouse;
	bool		EnableKeyboard;
	bool		ClampedToScreen;
	bool		Protected;
	
	u8	Padding[1];

private:

	SInset		HitRectInsets;
	std::list<uiFrame*>		Frames;			//child
	std::list<uiLayer*>		Layers;
	uiScriptContainer*		ScriptContainer;
	bool		InheritanceEnabled;

	u8		Padding2[3];
};

//parser
class uiParser_Frame
{
	DECLARE_INSTANCE(uiParser_Frame)

	typedef uiParser_LayoutFrame	BaseParser;

public:
	bool loadXml(const pugi::xml_node& node, uiFrame& control);

private:

	bool loadProperties(const pugi::xml_node& node, uiFrame& control);

	bool loadChild(const pugi::xml_node& node, uiFrame& control);
};