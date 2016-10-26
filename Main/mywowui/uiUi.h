#pragma once

#include "core.h"
#include "uibase.h"
#include "uiStructs.h"
#include "pugixml.hpp"

#include <vector>
#include <list>

class uiLayoutFrame;
class uiFont;

class uiUi
{
private:
	DISALLOW_COPY_AND_ASSIGN(uiUi);

public:
	uiUi();
	~uiUi();

public:
	bool contains(uiFont* font) const ;
	bool contains(uiLayoutFrame* layoutFrame) const;

	bool addFont(uiFont* font);
	bool addLayoutFrame(uiLayoutFrame* layoutFrame);

	bool removeFont(uiFont* font);
	bool removeLayoutFrame(uiLayoutFrame* layoutFrame);

public:
	std::vector<SInclude>		Includes;
	std::vector<SScriptBlock>		Scripts;

private:
	typedef std::list<uiLayoutFrame*, qzone_allocator<uiLayoutFrame*>>	T_LayoutFrameList;
	typedef std::list<uiFont*, qzone_allocator<uiFont*>>	T_FontList;

	T_LayoutFrameList	LayoutFrames;
	T_FontList		Fonts;

	bool	InheritanceEnabled;
	u8	Padding[3];
};

//parser
class uiParser_Ui 
{
	DECLARE_INSTANCE(uiParser_Ui)

//xml element
public:		
	static const c8* Include;
	static const c8* Script;
	static const c8* Font;
	static const c8* LayoutFrame;
	static const c8* Button;
	static const c8* CheckButton;
	static const c8* ColorSelect;
	static const c8* Cooldown;
	static const c8* DressUpModel;
	static const c8* EditBox;
	static const c8* FontString;
	static const c8* Frame;
	static const c8* GameTooltip;
	static const c8* MessageFrame;
	static const c8* Minimap;
	static const c8* Model;
	static const c8* ModelFFX;
	static const c8* MovieFrame;
	static const c8* PlayerModel;
	static const c8* ScrollFrame;
	static const c8* ScrollingMessageFrame;
	static const c8* SimpleHTML;
	static const c8* Slider;
	static const c8* StatusBar;
	static const c8* TabardModel;
	static const c8* TaxiRouteFrame;
	static const c8* Texture;
	static const c8* WorldFrame;

public:
	bool loadXml(const pugi::xml_node& node, uiUi& control);

private:
	bool loadProperties(const pugi::xml_node& node, uiUi& control);
	bool loadChild(const pugi::xml_node& node, uiUi& control);

private:
	bool load_Include(const pugi::xml_node& node, uiUi& control);
	bool load_Script(const pugi::xml_node& node, uiUi& control);
	bool load_Font(const pugi::xml_node& node, uiUi& control);
	bool load_LayoutFrame(const pugi::xml_node& node, uiUi& control);
	bool load_Frame(const pugi::xml_node& node, uiUi& control);
	bool load_Button(const pugi::xml_node& node, uiUi& control);
	bool load_CheckButton(const pugi::xml_node& node, uiUi& control);
	bool load_ColorSelect(const pugi::xml_node& node, uiUi& control);
	bool load_Cooldown(const pugi::xml_node& node, uiUi& control);
	bool load_DressUpModel(const pugi::xml_node& node, uiUi& control);
	bool load_EditBox(const pugi::xml_node& node, uiUi& control);
	bool load_FontString(const pugi::xml_node& node, uiUi& control);
	bool load_GameTooltip(const pugi::xml_node& node, uiUi& control);
	bool load_MessageFrame(const pugi::xml_node& node, uiUi& control);
	bool load_Minimap(const pugi::xml_node& node, uiUi& control);
	bool load_Model(const pugi::xml_node& node, uiUi& control);
	bool load_ModelFFX(const pugi::xml_node& node, uiUi& control);
	bool load_MovieFrame(const pugi::xml_node& node, uiUi& control);
	bool load_PlayerModel(const pugi::xml_node& node, uiUi& control);
	bool load_ScrollFrame(const pugi::xml_node& node, uiUi& control);
	bool load_ScrollingMessageFrame(const pugi::xml_node& node, uiUi& control);
	bool load_SimpleHTML(const pugi::xml_node& node, uiUi& control);
	bool load_Slider(const pugi::xml_node& node, uiUi& control);
	bool load_StatusBar(const pugi::xml_node& node, uiUi& control);
	bool load_TabardModel(const pugi::xml_node& node, uiUi& control);
	bool load_TaxiRouteFrame(const pugi::xml_node& node, uiUi& control);
	bool load_Texture(const pugi::xml_node& node, uiUi& control);
	bool load_WorldFrame(const pugi::xml_node& node, uiUi& control);
};