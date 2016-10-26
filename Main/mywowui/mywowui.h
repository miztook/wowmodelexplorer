#pragma once

#ifdef AUTOMATIC_LINK

#pragma comment(lib, "pugixml.lib") 

#endif

/*

LayoutFrame
		Frame
				Button
						CheckButton
				ColorSelect				
				Cooldown
				Model
						PlayerModel
								DressUpModel
								TabardModel
						ModelFFX
				EditBox
				GameTooltip
				MessageFrame
						ScrollingMessageFrame
				Minimap
				MovieFrame
				ScrollFrame
				SimpleHTML
				Slider
				StatusBar
				TaxiRouteFrame
				WorldFrame
		LayeredLayoutFrame
				FontString
				Texture

*/

#include "uiInheritableObject.h"
#include "uiStructs.h"
#include "uiButton.h"
#include "uiCheckButton.h"
#include "uiColorSelect.h"
#include "uiCooldown.h"
#include "uiDressUpModel.h"
#include "uiEditBox.h"
#include "uiFont.h"
#include "uiFontString.h"
#include "uiFrame.h"
#include "uiLayer.h"
#include "uiLayeredLayoutFrame.h"
#include "uiLayoutFrame.h"
#include "uiGameTooltip.h"
#include "uiMessageFrame.h"
#include "uiMinimap.h"
#include "uiModel.h"
#include "uiModelFFX.h"
#include "uiMovieFrame.h"
#include "uiPlayerModel.h"
#include "uiScriptContainer.h"
#include "uiScrollFrame.h"
#include "uiScrollingMessageFrame.h"
#include "uiSimpleHTML.h"
#include "uiSlider.h"
#include "uiStatusBar.h"
#include "uiTabardModel.h"
#include "uiTaxiRouteFrame.h"
#include "uiTexture.h"
#include "uiUi.h"
#include "uiWorldFrame.h"

#include "uiLoader.h"
#include "uiFrameXmlDirectory.h"
#include "uiSceneManager.h"

//parser
#include "uiParser_Struct.h"

void createUISystem();
void destroyUISystem();

#include "base.h"

class UISystem
{
private:
	DISALLOW_COPY_AND_ASSIGN(UISystem);

public:
	UISystem();
	~UISystem();

public:
	uiLoader* getUiLoader() const { return UILoader; }
	uiFrameXmlDirectory* getUiFrameXmlDirectory() const { return UIFrameXmlDirectory; }
	uiSceneManager* getUiSceneManager() const { return UISceneManager; }

private:
	uiLoader*	UILoader;
	uiFrameXmlDirectory*	UIFrameXmlDirectory;
	uiSceneManager*		UISceneManager;
};

extern UISystem* g_UISystem;
