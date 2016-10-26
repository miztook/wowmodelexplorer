#include "stdafx.h"
#include "uiUi.h"
#include "mywowui.h"

uiUi::uiUi()
: InheritanceEnabled(false)
{

}

uiUi::~uiUi()
{
	for (T_FontList::const_iterator itr = Fonts.begin(); itr != Fonts.end(); ++itr)
	{
		uiFont* font = (*itr);
		delete font;
	}
	Fonts.clear();

	for (T_LayoutFrameList::const_iterator itr = LayoutFrames.begin(); itr != LayoutFrames.end(); ++itr)
	{
		uiLayoutFrame* frame = (*itr);
		if(!frame->isVirtual())
			delete frame;
	}
	LayoutFrames.clear();
}

bool uiUi::contains( uiFont* font ) const
{
	for (T_FontList::const_iterator itr = Fonts.begin(); itr != Fonts.end(); ++itr)
	{
		if ((*itr) == font)
			return true;
	}
	return false;
}

bool uiUi::contains( uiLayoutFrame* layoutFrame ) const
{
	for (T_LayoutFrameList::const_iterator itr = LayoutFrames.begin(); itr != LayoutFrames.end(); ++itr)
	{
		if ((*itr) == layoutFrame)
			return true;
	}
	return false;
}

bool uiUi::addFont( uiFont* font )
{
	Fonts.push_back(font);

	return true;
}

bool uiUi::addLayoutFrame( uiLayoutFrame* layoutFrame )
{
	LayoutFrames.push_back(layoutFrame);

	return true;
}

bool uiUi::removeFont( uiFont* font )
{
	for (T_FontList::iterator itr = Fonts.begin(); itr != Fonts.end(); ++itr)
	{
		if ((*itr) == font)
		{
			Fonts.erase(itr);
			delete font;
			break;
		}
	}
	return false;
}

bool uiUi::removeLayoutFrame( uiLayoutFrame* layoutFrame )
{
	for (T_LayoutFrameList::iterator itr = LayoutFrames.begin(); itr != LayoutFrames.end(); ++itr)
	{
		if ((*itr) == layoutFrame)
		{
			LayoutFrames.erase(itr);
			delete layoutFrame;
			break;
		}
	}
	return false;
}

//parser
const c8* uiParser_Ui::Include = "Include";
const c8* uiParser_Ui::Script = "Script";
const c8* uiParser_Ui::Font = "Font";
const c8* uiParser_Ui::LayoutFrame = "LayoutFrame";
const c8* uiParser_Ui::Button = "Button";
const c8* uiParser_Ui::CheckButton = "CheckButton";
const c8* uiParser_Ui::ColorSelect = "ColorSelect";
const c8* uiParser_Ui::Cooldown = "Cooldown";
const c8* uiParser_Ui::DressUpModel = "DressUpModel";
const c8* uiParser_Ui::EditBox = "EditBox";
const c8* uiParser_Ui::FontString = "FontString";
const c8* uiParser_Ui::Frame = "Frame";
const c8* uiParser_Ui::GameTooltip = "GameTooltip";
const c8* uiParser_Ui::MessageFrame = "MessageFrame";
const c8* uiParser_Ui::Minimap = "Minimap";
const c8* uiParser_Ui::Model = "Model";
const c8* uiParser_Ui::ModelFFX = "ModelFFX";
const c8* uiParser_Ui::MovieFrame = "MovieFrame";
const c8* uiParser_Ui::PlayerModel = "PlayerModel";
const c8* uiParser_Ui::ScrollFrame = "ScrollFrame";
const c8* uiParser_Ui::ScrollingMessageFrame = "ScrollingMessageFrame";
const c8* uiParser_Ui::SimpleHTML = "SimpleHTML";
const c8* uiParser_Ui::Slider = "Slider";
const c8* uiParser_Ui::StatusBar = "StatusBar";
const c8* uiParser_Ui::TabardModel = "TabardModel";
const c8* uiParser_Ui::TaxiRouteFrame = "TaxiRouteFrame";
const c8* uiParser_Ui::Texture = "Texture";
const c8* uiParser_Ui::WorldFrame = "WorldFrame";

bool uiParser_Ui::loadXml( const pugi::xml_node& node, uiUi& control )
{
	if (!loadProperties(node, control))
	{
		_ASSERT(false);
		return false;
	}

	for (pugi::xml_node child = node.first_child(); child; child = child.next_sibling())
	{
		if (!loadChild(child, control))
		{
			_ASSERT(false);
			return false;
		}
	}

	return true;
}

bool uiParser_Ui::loadProperties( const pugi::xml_node& node, uiUi& control )
{
	return true;
}

bool uiParser_Ui::loadChild( const pugi::xml_node& node, uiUi& control )
{
	string64 name = node.name();

	if (name == uiParser_Ui::Include)
	{
		load_Include(node, control);
	}
	else if(name == uiParser_Ui::Script)
	{
		load_Script(node, control);
	}
	else if (name == uiParser_Ui::Font)
	{
		load_Font(node, control);
	}
	else if (name == uiParser_Ui::LayoutFrame)
	{
		load_LayoutFrame(node, control);
	}
	else if (name == uiParser_Ui::Button)
	{
		load_Button(node, control);
	}
	else if (name == uiParser_Ui::CheckButton)
	{
		load_CheckButton(node, control);
	}
	else if (name == uiParser_Ui::ColorSelect)
	{
		load_ColorSelect(node, control);
	}
	else if (name == uiParser_Ui::Cooldown)
	{
		load_Cooldown(node, control);
	}
	else if (name == uiParser_Ui::DressUpModel)
	{
		load_DressUpModel(node, control);
	}
	else if (name == uiParser_Ui::EditBox)
	{
		load_EditBox(node, control);
	}
	else if (name == uiParser_Ui::FontString)
	{
		load_FontString(node, control);
	}
	else if (name == uiParser_Ui::Frame)
	{
		load_Frame(node, control);
	}
	else if (name == uiParser_Ui::GameTooltip)
	{
		load_GameTooltip(node, control);
	}
	else if (name == uiParser_Ui::MessageFrame)
	{
		load_MessageFrame(node, control);
	}
	else if (name == uiParser_Ui::Minimap)
	{
		load_Minimap(node, control);
	}
	else if (name == uiParser_Ui::Model)
	{
		load_Model(node, control);
	}
	else if (name == uiParser_Ui::ModelFFX)
	{
		load_ModelFFX(node, control);
	}
	else if (name == uiParser_Ui::MovieFrame)
	{
		load_MovieFrame(node, control);
	}
	else if (name == uiParser_Ui::PlayerModel)
	{
		load_PlayerModel(node, control);
	}
	else if (name == uiParser_Ui::ScrollFrame)
	{
		load_ScrollFrame(node, control);
	}
	else if (name == uiParser_Ui::ScrollingMessageFrame)
	{
		load_ScrollingMessageFrame(node, control);
	}
	else if (name == uiParser_Ui::SimpleHTML)
	{
		load_SimpleHTML(node, control);
	}
	else if (name == uiParser_Ui::Slider)
	{
		load_Slider(node, control);
	}
	else if (name == uiParser_Ui::StatusBar)
	{
		load_StatusBar(node, control);
	}
	else if (name == uiParser_Ui::TabardModel)
	{
		load_TabardModel(node, control);
	}
	else if (name == uiParser_Ui::TaxiRouteFrame)
	{
		load_TaxiRouteFrame(node, control);
	}
	else if (name == uiParser_Ui::Texture)
	{
		load_Texture(node, control);
	}
	else if (name == uiParser_Ui::WorldFrame)
	{
		load_WorldFrame(node, control);
	}
	else
	{
		CSysUtility::outputDebug("uiParser_Ui::loadChild unknown node: %s", name.c_str());		
		_ASSERT(false);
	}

	return true;
}

bool uiParser_Ui::load_Include( const pugi::xml_node& node, uiUi& control )
{
	SInclude include;
	bool success = uiParser_SInclude::getInstance().loadXml(node, include);

	control.Includes.push_back(include);

	return success;
}

bool uiParser_Ui::load_Script( const pugi::xml_node& node, uiUi& control )
{
	SScriptBlock scriptBlock;
	bool success = uiParser_SScriptBlock::getInstance().loadXml(node, scriptBlock);

	control.Scripts.push_back(scriptBlock);

	return success;
}

bool uiParser_Ui::load_Font( const pugi::xml_node& node, uiUi& control )
{
	uiFont* font = new uiFont;
	bool success =uiParser_Font::getInstance().loadXml(node, *font);

	control.addFont(font);

	return success;
}

bool uiParser_Ui::load_LayoutFrame( const pugi::xml_node& node, uiUi& control )
{
	uiLayoutFrame* layoutframe = new uiLayoutFrame;
	bool success = uiParser_LayoutFrame::getInstance().loadXml(node, *layoutframe);

	control.addLayoutFrame(layoutframe);

	return success;
}

bool uiParser_Ui::load_Frame(const pugi::xml_node& node, uiUi& control)
{
	uiFrame* frame = new uiFrame;
	bool success = uiParser_Frame::getInstance().loadXml(node, *frame);

	control.addLayoutFrame(frame);

	return success;
}

bool uiParser_Ui::load_Button( const pugi::xml_node& node, uiUi& control )
{
	uiButton* button = new uiButton;
	bool success = uiParser_Button::getInstance().loadXml(node, *button);

	control.addLayoutFrame(button);

	return success;
}

bool uiParser_Ui::load_CheckButton( const pugi::xml_node& node, uiUi& control )
{
	uiCheckButton* checkButton = new uiCheckButton;
	bool success = uiParser_CheckButton::getInstance().loadXml(node, *checkButton);

	control.addLayoutFrame(checkButton);

	return success;
}

bool uiParser_Ui::load_ColorSelect( const pugi::xml_node& node, uiUi& control )
{
	uiColorSelect* colorSelect = new uiColorSelect;
	bool success = uiParser_ColorSelect::getInstance().loadXml(node, *colorSelect);

	control.addLayoutFrame(colorSelect);

	return success;
}

bool uiParser_Ui::load_Cooldown( const pugi::xml_node& node, uiUi& control )
{
	uiCooldown* cooldown = new uiCooldown;
	bool success = uiParser_Cooldown::getInstance().loadXml(node, *cooldown);

	control.addLayoutFrame(cooldown);

	return success;
}

bool uiParser_Ui::load_DressUpModel( const pugi::xml_node& node, uiUi& control )
{
	uiDressUpModel* dressUpModel = new uiDressUpModel;
	bool success = uiParser_DressUpModel::getInstance().loadXml(node, *dressUpModel);

	control.addLayoutFrame(dressUpModel);

	return success;
}

bool uiParser_Ui::load_EditBox( const pugi::xml_node& node, uiUi& control )
{
	uiEditBox* editBox = new uiEditBox;
	bool success = uiParser_EditBox::getInstance().loadXml(node, *editBox);

	control.addLayoutFrame(editBox);

	return success;
}

bool uiParser_Ui::load_FontString( const pugi::xml_node& node, uiUi& control )
{
	uiFontString* fontString = new uiFontString;
	bool success = uiParser_FontString::getInstance().loadXml(node, *fontString);

	control.addLayoutFrame(fontString);

	return success;
}

bool uiParser_Ui::load_GameTooltip(const pugi::xml_node& node, uiUi& control)
{
	uiGameTooltip* gametooltip = new uiGameTooltip;
	bool success = uiParser_GameTooltip::getInstance().loadXml(node, *gametooltip);

	control.addLayoutFrame(gametooltip);

	return success;
}

bool uiParser_Ui::load_MessageFrame(const pugi::xml_node& node, uiUi& control)
{
	uiMessageFrame* messageFrame = new uiMessageFrame;
	bool success = uiParser_MessageFrame::getInstance().loadXml(node, *messageFrame);

	control.addLayoutFrame(messageFrame);

	return success;
}

bool uiParser_Ui::load_Minimap(const pugi::xml_node& node, uiUi& control)
{
	uiMinimap* minimap = new uiMinimap;
	bool success = uiParser_Minimap::getInstance().loadXml(node, *minimap);

	control.addLayoutFrame(minimap);

	return success;
}

bool uiParser_Ui::load_Model(const pugi::xml_node& node, uiUi& control)
{
	uiModel* model = new uiModel;
	bool success = uiParser_Model::getInstance().loadXml(node, *model);

	control.addLayoutFrame(model);

	return success;
}

bool uiParser_Ui::load_ModelFFX(const pugi::xml_node& node, uiUi& control)
{
	uiModelFFX* model = new uiModelFFX;
	bool success = uiParser_ModelFFX::getInstance().loadXml(node, *model);

	control.addLayoutFrame(model);

	return success;
}

bool uiParser_Ui::load_MovieFrame(const pugi::xml_node& node, uiUi& control)
{
	uiMovieFrame* movieFrame = new uiMovieFrame;
	bool success = uiParser_MovieFrame::getInstance().loadXml(node, *movieFrame);

	control.addLayoutFrame(movieFrame);

	return success;
}

bool uiParser_Ui::load_PlayerModel(const pugi::xml_node& node, uiUi& control)
{
	uiPlayerModel* playerModel = new uiPlayerModel;
	bool success = uiParser_PlayerModel::getInstance().loadXml(node, *playerModel);

	control.addLayoutFrame(playerModel);

	return success;
}

bool uiParser_Ui::load_ScrollFrame(const pugi::xml_node& node, uiUi& control)
{
	uiScrollFrame* scrollFrame = new uiScrollFrame;
	bool success = uiParser_ScrollFrame::getInstance().loadXml(node, *scrollFrame);

	control.addLayoutFrame(scrollFrame);

	return success;
}

bool uiParser_Ui::load_ScrollingMessageFrame(const pugi::xml_node& node, uiUi& control)
{
	uiScrollingMessageFrame* scrollingMessageFrame = new uiScrollingMessageFrame;
	bool success = uiParser_ScrollingMessageFrame::getInstance().loadXml(node, *scrollingMessageFrame);

	control.addLayoutFrame(scrollingMessageFrame);

	return success;
}

bool uiParser_Ui::load_SimpleHTML(const pugi::xml_node& node, uiUi& control)
{
	uiSimpleHTML* simpleHTML = new uiSimpleHTML;
	bool success = uiParser_SimpleHTML::getInstance().loadXml(node, *simpleHTML);

	control.addLayoutFrame(simpleHTML);

	return success;
}

bool uiParser_Ui::load_Slider(const pugi::xml_node& node, uiUi& control)
{
	uiSlider* slider = new uiSlider;
	bool success = uiParser_Slider::getInstance().loadXml(node, *slider);

	control.addLayoutFrame(slider);

	return success;
}

bool uiParser_Ui::load_StatusBar(const pugi::xml_node& node, uiUi& control)
{
	uiStatusBar* statusBar = new uiStatusBar;
	bool success = uiParser_StatusBar::getInstance().loadXml(node, *statusBar);

	control.addLayoutFrame(statusBar);

	return success;
}

bool uiParser_Ui::load_TabardModel(const pugi::xml_node& node, uiUi& control)
{
	uiTabardModel* tabardModel = new uiTabardModel;
	bool success = uiParser_TabardModel::getInstance().loadXml(node, *tabardModel);

	control.addLayoutFrame(tabardModel);

	return success;
}

bool uiParser_Ui::load_TaxiRouteFrame(const pugi::xml_node& node, uiUi& control)
{
	uiTaxiRouteFrame* taxiRouteFrame = new uiTaxiRouteFrame;
	bool success = uiParser_TaxiRouteFrame::getInstance().loadXml(node, *taxiRouteFrame);

	control.addLayoutFrame(taxiRouteFrame);

	return success;
}

bool uiParser_Ui::load_Texture(const pugi::xml_node& node, uiUi& control)
{
	uiTexture* texture = new uiTexture;
	bool success = uiParser_Texture::getInstance().loadXml(node, *texture);

	control.addLayoutFrame(texture);

	return success;
}

bool uiParser_Ui::load_WorldFrame(const pugi::xml_node& node, uiUi& control)
{
	uiWorldFrame* worldframe = new uiWorldFrame;
	bool success = uiParser_WorldFrame::getInstance().loadXml(node, *worldframe);

	control.addLayoutFrame(worldframe);

	return success;
}

