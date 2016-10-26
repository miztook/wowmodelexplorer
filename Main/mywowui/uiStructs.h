#pragma once

#include "core.h"
#include "uibase.h"
#include "SColor.h"

struct SAbsValue
{
	SAbsValue() : Value(0) {}

	s32	Value;
};

struct SAnchor
{
	E_FRAME_POINT	Point;
	E_FRAME_POINT	RelativePoint;
	string64	RelativeTo;
};

struct SInset
{
	SInset() : Left(0), Right(0), Top(0), Bottom(0) {}

	s32	Left;
	s32	Right;
	s32	Top;
	s32	Bottom;
};

struct STexCoords
{
	STexCoords() : Left(0.0f), Right(0.0f), Top(0.0f), Bottom(0.0f) {}

	f32	Left;
	f32	Right;
	f32	Top;
	f32	Bottom;
};

struct SBackDrop
{
	SBackDrop() : Tile(false) {}

	string256		BackGroundFile;
	string256		EdgeFile;
	SInset		BackgroundInset;
	SAbsValue		TileSize;
	SAbsValue		EdgeSize;
	SColor		Color;
	SColor		BorderColor;	
	bool			Tile;
	u8				Padding[3];
};

struct SResizeBounds
{
	dimension2du MinResize;
	dimension2du MaxResize;
};

struct SInclude
{
	string256	File;
};

struct SScriptBlock
{
	SScriptBlock() : ScriptBlock(NULL), ScriptLength(0) {}

	string256	File;
	u8*		ScriptBlock;
	u32		ScriptLength;
};

struct SButtonStyle
{
	string256 Style;
};

struct SAbsDimension
{
	SAbsDimension() : X(0), Y(0) {}

	int X;
	int Y;
};

struct SRelDimension
{
	SRelDimension() : X(0.0f), Y(0.0f) {}

	float X;
	float Y;
};

struct SDimension
{
	SAbsDimension	AbsDimension;
	SRelDimension		RelDimension;
};

struct SFontHeight 
{
	SAbsValue	AbsValue;
};

struct SShadow
{
	SDimension	Offset;
	SColorf	Color;
};