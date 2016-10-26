#pragma once

enum E_CONTROL_TYPE
{
	ControlType_Unknown = 0,
	ControlType_Ui,
	ControlType_LayoutFrame,
	ControlType_Button,
	ControlType_CheckButton,
	ControlType_ColorSelect,
	ControlType_Cooldown,
	ControlType_DressUpModel,
	ControlType_EditBox,
	ControlType_FontString,
	ControlType_Frame,
	ControlType_GameTooltip,
	ControlType_MessageFrame,
	ControlType_Minimap,
	ControlType_Model,
	ControlType_ModelFFX,
	ControlType_MovieFrame,
	ControlType_PlayerModel,
	ControlType_ScrollFrame,
	ControlType_ScrollingMessageFrame,
	ControlType_SimpleHTML,
	ControlType_Slider,
	ControlType_StatusBar,
	ControlType_TabardModel,
	ControlType_TaxiRouteFrame,
	ControlType_Texture,
	ControlType_WorldFrame,

	//struct
	ControlType_Include,
	ControlType_Script,
	ControlType_Font,

	ControlTypeCount,

	ControlType_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_FRAME_POINT
{
	EFP_TOP_LEFT = 0,
	EFP_TOP_RIGHT,
	EFP_BOTTOM_LEFT,
	EFP_BOTTOM_RIGHT,
	EFP_TOP,
	EFP_BOTTOM,
	EFP_LEFT,
	EFP_RIGHT,
	EFP_CETNER,

	EFP_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_FRAME_STRATA
{
	EFS_PARENT = 0,
	EFS_BACKGROUND,
	EFS_LOW,
	EFS_MEDIUM,
	EFS_HIGH,
	EFS_DIALOG,
	EFS_FULLSCREEN,
	EFS_FULLSCREENDIALOG,
	EFS_TOOLTIP,

	EFS_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_DRAW_LAYER
{
	EDL_BACKGROUND = 0,
	EDL_BORDER,
	EDL_ARTWORK,
	EDL_OVERLAY,
	EDL_HILIGHT,

	EDL_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_ALPHA_MODE
{
	EAM_DISABLE = 0,
	EAM_BLEND,
	EAM_ALPHAKEY,
	EAM_ADD,
	EAM_MOD,

	EAM_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_OUTLINE
{
	Outline_None = 0,
	Outline_Normal,
	Outline_Thick,

	Outline_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

inline E_OUTLINE E_OUTLINE_Parse(const c8* str)
{
	if (str == "NORMAL")
		return Outline_Normal;
	else if (str == "THICK")
		return Outline_Thick;
	else
		return Outline_None;
}

enum E_JUSTIFY_VERTICAL
{
	JustifyVertical_Top = 0,
	JustifyVertical_Middle,
	JustifyVertical_Bottom,

	JustifyVertical_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_JUSTIFY_HORIZONTAL
{
	JustifyHorizontal_Left = 0,
	JustifyHorizontal_Center,
	JustifyHorizontal_Right,

	JustifyHorizontal_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_ORIENTATION
{
	Orientation_Horizontal = 0,
	Orientation_Vertical,

	Orientation_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_INSERT_MODE
{
	InsertMode_Top = 0,
	InsertMode_Bottom,

	InsertMode_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};