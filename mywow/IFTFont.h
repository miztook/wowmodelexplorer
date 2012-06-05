#pragma once

#include "base.h"
#include "SColor.h"
#include "rect.h"

class ITexture;

class IFTFont
{
public:
	virtual ~IFTFont() { }

public:
	virtual void drawA(c8* text, SColor color, vector2di position, u32 glyphPadding=1, u32 linePadding=2) = 0;
	virtual void drawW(c16* text, SColor color, vector2di position, u32 glyphPadding=1, u32 linePadding=2) = 0;
	virtual dimension2du getDimension(c16* text) = 0;
	virtual ITexture* getTexture(u32 idx) = 0;
	virtual u32 getTextureCount() const = 0;
};