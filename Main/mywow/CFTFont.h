#pragma once

#include "IFTFont.h"
#include "S3DVertex.h"
#include <vector>
#include <unordered_map>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

class CFontServices;
class CFTGlyphCache;
struct S2DBlendParam;

class CFTFont : public IFTFont
{
public:
	CFTFont(const char* faceName, int faceIndex, uint32_t size, int fontStyle, int outlineWidth);
	~CFTFont();

public:
	virtual void drawA(const char* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = nullptr);
	virtual void drawW(const char16_t* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = nullptr);
	virtual void addTextA(const char* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = nullptr, bool bVertical = false);
	virtual void addTextW(const char16_t* text, SColor color, vector2di position, int nCharCount = -1, recti* pClip = nullptr, bool bVertical = false);
	virtual void flushText();

	virtual dimension2du getTextExtent(const char* utf8text, int nCharCount = -1, bool vertical = false);	
	virtual dimension2du getWTextExtent(const char16_t* text, int nCharCount = -1, bool vertical = false) ;	
	virtual dimension2du getWCharExtent(char16_t ch, bool vertical = false);
	virtual int GetTextWCount(const char* utf8text) const;

	FT_BitmapGlyph getCharExtent(uint32_t ch, int& offsetX, int& offsetY, int& chWidth, int& chHeight, int& chBmpW, int& chBmpH);
	const SCharInfo* getCharInfo(char16_t ch) const;
	const SCharInfo* addChar(char16_t ch);	
	ITexture* getTexture(uint32_t idx);
	uint32_t getTextureCount() const { return (uint32_t)FontTextures.size(); }

public:
	bool init();

	int GetUnicodeCMapIndex(FT_Face ftFace);
	My_FaceID GetMyFaceID() { return MyFaceID; }
	FTC_Scaler GetFTCScaler() { return FTCScaler; }
	FT_Face GetFTFace();
	FT_Size GetFTSize();
	FT_Pos GetBoldWidth(){ return BoldWidth; }

private:
	struct SDrawText
	{
		uint32_t		offset;
		uint32_t		length;
		SColor color;
		vector2di position;
		recti		rcClip;
		bool		bClip;
		bool		bVertical;
		//TODO underline
	};

private:
	bool addFontTexture();
	FT_BitmapGlyph RenderChar(uint32_t ch);
	void drawTextWBatch();

	void drawText(const SDrawText& d, const char16_t* txt, float fInv, const S2DBlendParam& blendParam);
	void drawTextVertical(const SDrawText& d, const char16_t* txt, float fInv, const S2DBlendParam& blendParam);

private:
#ifdef USE_QALLOCATOR
	typedef std::map<char16_t, SCharInfo, std::less<char16_t>, qzone_allocator<std::pair<char16_t, SCharInfo>>>	T_CharacterMap;
#else
	typedef std::unordered_map<char16_t, SCharInfo>	T_CharacterMap;
#endif

	T_CharacterMap		CharacterMap;

private:
	CFontServices*		FontServices;
	CFTGlyphCache*		MyFTGlyphCache;

	My_FaceID	MyFaceID;
	FTC_Scaler	FTCScaler;
	FT_Pos		BoldWidth;
	int		UnicodeCMapIndex;

	int		Ascender;				//overhang
	int		Descender;			//underhang

	std::vector<ITexture*>			FontTextures;

	int	NextX;
	int  NextY;
	int  CurrentX;
	int  CurrentY;
	int  MaxY;

	//
	std::vector<char16_t>		Texts;
	std::vector<SDrawText>	DrawTexts;
};