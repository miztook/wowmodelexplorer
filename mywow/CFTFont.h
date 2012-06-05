#pragma once

#include "IFTFont.h"
#include <vector>
#include <map>

struct SCharInfo
{
	SCharInfo() : TexIndex(-1), offsetX(0), offsetY(0) {}

	recti	UVRect;
	s32		TexIndex;
	s32		offsetX;
	s32		offsetY;
};

struct SDrawBatch
{
	vector2di	positions[MAX_TEXT_LENGTH];
	recti*		sourceRects[MAX_TEXT_LENGTH];
	ITexture*	texture;
};

class CFTFont : public IFTFont
{
public:
	CFTFont(const char* faceName, u32 size);
	~CFTFont();

public:
	virtual void drawA(c8* text, SColor color, vector2di position, u32 glyphPadding=1, u32 linePadding=2);
	virtual void drawW(c16* text, SColor color, vector2di position, u32 glyphPadding=1, u32 linePadding=2);
	virtual dimension2du getDimension(c16* text);
	virtual ITexture* getTexture(u32 idx);
	virtual u32 getTextureCount() const { return DrawBatches.size(); }

	void addChar(c16 ch);

public:
	typedef std::map<c16, SCharInfo, std::less<c16>, qzone_allocator<std::pair<c16,SCharInfo>>>	T_CharacterMap;
	T_CharacterMap		CharacterMap;

private:
	FT_Library		ftLibrary;
	FT_Face		ftFace;
	FT_F26Dot6		ftSize;

	u32		Ascender;				//overhang
	u32		Descender;			//underhang
	s32		FontHeight;			//字体最大高度

	std::vector<SDrawBatch>			DrawBatches;

	//
	s32	NextX;
	s32  NextY;
	s32  CurrentX;
	s32  CurrentY;
	s32  MaxY;

	//
	c16 Tmp[512];
private:
	bool addFontTexture();
};