#pragma once

#include "base.h"
#include "SColor.h"
#include "fixstring.h"
#include "dimension2d.h"
#include "fontDef.h"

class ITexture;

class IFTFont
{
public:
	explicit IFTFont(const char* faceName, int faceIndex, u32 size, int fontStyle, int outlineWidth) 
		: FontFacePath(faceName), FontFaceIndex(faceIndex), FontSize(size), FontStyle(fontStyle), OutlineWidth(outlineWidth)
	{
		m_iFontWidth = m_iFontHeight = 0;
		m_bFixedSize = false;
	}
	virtual ~IFTFont() { }

public:
	u32 getFontSize() const { return FontSize; }
	const char* getFontFacePath() const { return FontFacePath.c_str(); }
	int getFontFaceIndex() const { return FontFaceIndex; }
	int getFontStyle() const { return FontStyle; }
	int getOutlineWidth() const { return OutlineWidth; }
	int getFontWidth() const { return m_iFontWidth; }
	int getFontHeight() const { return m_iFontHeight; }
	bool isFixedSize() const { return m_bFixedSize; }
	void setFixedSize(bool bFixed) { m_bFixedSize = bFixed; }

public:
	struct SCharInfo
	{
		SCharInfo() : TexIndex(-1), offsetX(0), offsetY(0), width(0), height(0) {}

		recti		UVRect;		//in texture
		int		TexIndex;
		s16		offsetX;
		s16		offsetY;
		s16		width;
		s16		height;
	};

public:
	virtual void drawA(const char* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = NULL_PTR) = 0;
	virtual void drawW(const c16* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = NULL_PTR) = 0;
	virtual void addTextA(const char* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = NULL_PTR, bool bVertical = false) = 0;
	virtual void addTextW(const c16* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = NULL_PTR, bool bVertical = false) = 0;
	virtual void flushText() = 0;

	virtual dimension2du getTextExtent(const char* utf8text, int nCharCount = -1, bool vertical = false) = 0;	
	virtual dimension2du getWTextExtent(const c16* text, int nCharCount = -1, bool vertical = false) = 0;	
	virtual dimension2du getWCharExtent(c16 ch, bool vertical = false) = 0;
	virtual int GetTextWCount(const char* utf8text) const = 0;

protected:
	string256	FontFacePath;
	int FontFaceIndex;
	u32 FontSize;
	int FontStyle;
	int OutlineWidth;

	//字体最大高度宽度
	int		m_iFontWidth;
	int		m_iFontHeight;

	bool		m_bFixedSize;
};