#include "stdafx.h"
#include "CFTFont.h"
#include "mywow.h"
#include "CFontServices.h"
#include "CFTGlyphCache.h"

CFTFont::CFTFont(const char* faceName, int faceIndex, uint32_t size, int fontStyle, int outlineWidth)
	: IFTFont(faceName, faceIndex, size, fontStyle, outlineWidth)
{
	Ascender =
		Descender = 0;

	NextX = NextY = CurrentX = CurrentY = MaxY = 0;

	FontServices = static_cast<CFontServices*>(g_Engine->getFontServices());
	MyFTGlyphCache = nullptr;

	MyFaceID = nullptr;
	FTCScaler = nullptr;
	UnicodeCMapIndex = -1;
	BoldWidth = 0;

	Texts.reserve(1024);
	DrawTexts.reserve(32);
}

bool CFTFont::init()
{
	if (FontSize == 18)
	{
		int x = 0;
	}

	MyFaceID = FontServices->LookupFaceID(FontFacePath.c_str(), FontFaceIndex, FontStyle);

	FT_Face ftFace;

	if (FTC_Manager_LookupFace(FontServices->GetCacheManager(), (FTC_FaceID)MyFaceID, &ftFace))
	{
		ASSERT(false);
		return false;
	}

	if (-1 == (UnicodeCMapIndex = GetUnicodeCMapIndex(ftFace)))
	{
		ASSERT(false);
		return false;
	}

	//
	FTCScaler = new FTC_ScalerRec;
	FTCScaler->face_id = (FTC_FaceID) MyFaceID;
	FTCScaler->width = (FontSize << 6);
	FTCScaler->height = (FontSize << 6);
	FTCScaler->pixel = 0;
	FTCScaler->x_res = DEFAULT_DPI;
	FTCScaler->y_res = DEFAULT_DPI;

	FT_Size ftSize;
	if (FTC_Manager_LookupSize(FontServices->GetCacheManager(), FTCScaler, &ftSize))
	{
		ASSERT(false);
		return false;
	}

	MyFTGlyphCache = new CFTGlyphCache(this);

	if(FontStyle & STYLE_BOLD)
	{
		// fix bold font glyph size. - Silas, 2013/03/08
		BoldWidth = FT_MulFix( ftFace->units_per_EM,
			ftSize->metrics.y_scale ) / 24;
	}
	else
	{
		BoldWidth = 0;
	}

	Ascender = (int)(((ftSize->metrics.ascender << 1) + BoldWidth) >> 7) + getOutlineWidth();
	Descender = (int)(((ftSize->metrics.descender << 1) + BoldWidth) >> 7) + getOutlineWidth();  // 基线到字符轮廓最低点的距离

	m_iFontWidth = (((ftFace->bbox.xMax - ftFace->bbox.xMin) * ftSize->metrics.x_scale) >> 22) + getOutlineWidth() * 2 + (BoldWidth >> 6);
	m_iFontHeight = (((ftFace->bbox.yMax - ftFace->bbox.yMin) * ftSize->metrics.y_scale) >> 22) + getOutlineWidth() * 2 + (BoldWidth >> 6);

	int offsetX, offsetY, chW, chH, bmpW, bmpH;
	if (getCharExtent(FontServices->getWidestChar(), offsetX, offsetY, chW, chH, bmpW, bmpH))
	{
		if (chW > m_iFontWidth)
			m_iFontWidth = bmpW;

		if (chH > m_iFontHeight)
			m_iFontHeight = bmpH;
	}

	addFontTexture();

	return true;
}

CFTFont::~CFTFont()
{
	for (uint32_t i=0; i<(uint32_t)FontTextures.size(); ++i)
	{
		if(FontTextures[i])
		{
			g_Engine->getTextureWriteServices()->removeTextureWriter(FontTextures[i]);
			FontTextures[i]->drop();
		}
	}

	delete MyFTGlyphCache;
	delete FTCScaler;
	if (MyFaceID)
		FontServices->RemoveFaceID(MyFaceID);
}

dimension2du CFTFont::getWTextExtent( const char16_t* text, int nCharCount, bool vertical )
{
	int x;
	int y;

	if (vertical)
	{
		x = m_iFontWidth;
		y = 0;
	}
	else
	{
		x = 0;
		y = m_iFontHeight;
	}

	uint32_t len = nCharCount > 0 ? (uint32_t)nCharCount : (uint32_t)Q_UTF16Len(text);
	for (uint32_t i=0; i<len; ++i)
	{
		char16_t ch = text[i];
		const IFTFont::SCharInfo* charInfo = addChar(ch);
		ASSERT(charInfo);

		if ( ch == (char16_t)'\r' || ch == (char16_t)'\n') // Unix breaks
		{
			if (vertical)
			{
				x += charInfo->width;
				y = 0;
			}
			else
			{
				y += m_iFontHeight;
				x = 0;
			}
		}
		else
		{
			if (vertical)
				y += m_iFontHeight;
			else
				x += charInfo->width;
		}
	}

	return dimension2du(x, y);
}

dimension2du CFTFont::getWCharExtent( char16_t ch, bool vertical )
{
	int x;
	int y;

	if (vertical)
	{
		x = m_iFontWidth;
		y = 0;
	}
	else
	{
		x = 0;
		y = m_iFontHeight;
	}

	const IFTFont::SCharInfo* charInfo = addChar(ch);
	ASSERT(charInfo);

	if ( ch == (char16_t)'\r' || ch == (char16_t)'\n') // Mac or Windows breaks
	{
		// 		if (vertical)
		// 		{
		// 			x += charInfo->width;
		// 			y = 0;
		// 		}
		// 		else
		y += m_iFontHeight;
		x = 0;
	}
	else
	{
		if (vertical)
			y += m_iFontHeight;
		else
			x += charInfo->width;
	}

	return dimension2du(x, y);
}

int CFTFont::GetTextWCount(const char* utf8text) const
{
	int uCharCount = 0;
	const char* p = utf8text;
	while(*p)
	{
		char16_t ch;
		int nadv;
		ch = (char16_t)Q_ParseUnicodeFromUTF8Str(p, &nadv);
		if(nadv == 0)		//parse end
			break;
		p += nadv;
		if (ch == 0)		//string end
			break;
		++uCharCount;
	}

	return uCharCount;
}

dimension2du CFTFont::getTextExtent( const char* utf8text, int nCharCount, bool vertical )
{
	int x;
	int y;

	if (vertical)
	{
		x = m_iFontWidth;
		y = 0;
	}
	else
	{
		x = 0;
		y = m_iFontHeight;
	}

	const char* sz8 = utf8text;
	int nLen = nCharCount > 0 ? nCharCount : (int)strlen(utf8text);
	while (*sz8 && nLen >= 0)
	{
		int nAdv;
		int value = Q_ParseUnicodeFromUTF8Str(sz8, &nAdv, nLen);
		if(nAdv == 0)
			break;
		sz8 += nAdv;
		nLen -= nAdv;
		if(value < 0 || value >= 0x10000)
			continue;

		char16_t ch = (char16_t)value;

		const IFTFont::SCharInfo* charInfo = addChar(ch);
		ASSERT(charInfo);

		if ( ch == (char16_t)'\r' || ch == (char16_t)'\n') // Unix breaks
		{
			// 			if (vertical)
			// 			{
			// 				x += charInfo->width;
			// 				y = 0;
			// 			}
			y += m_iFontHeight;
			x = 0;
		}
		else
		{
			if (vertical)
				y += m_iFontHeight;
			else
				x += charInfo->width;
		}
	}

	return dimension2du(x, y);
}

ITexture* CFTFont::getTexture( uint32_t idx )
{
	if (idx >= FontTextures.size())
		return nullptr;

	return FontTextures[idx];
}

const IFTFont::SCharInfo* CFTFont::addChar( char16_t ch )
{
	T_CharacterMap::const_iterator itr = CharacterMap.find(ch);
	if (itr != CharacterMap.end())
	{
		ASSERT((itr->second).TexIndex >= 0 && 
			(itr->second).TexIndex <= (int)FontTextures.size());
		return &itr->second;
	}

	FT_BitmapGlyph bitmapGlyph;
	int offsetX, offsetY, chW, chH, bmpW, bmpH;

	bitmapGlyph = getCharExtent(ch, offsetX, offsetY, chW, chH, bmpW, bmpH);
	if (!bitmapGlyph)
	{
		ASSERT(false);
		return nullptr;
	}

	NextX = CurrentX + m_iFontWidth;

	if (NextX > FONT_TEXTURE_SIZE)				//换行
	{
		CurrentX = 0;
		NextX = CurrentX + m_iFontWidth;
		CurrentY = NextY;
	}

	MaxY = CurrentY + m_iFontHeight;						//换页, 增加纹理
	if (MaxY > FONT_TEXTURE_SIZE)
	{
		addFontTexture();

		CurrentX = CurrentY = NextY = 0;
		MaxY = 0;
	}

	//write texture
	ITexture* tex = FontTextures.back();

	int charposx = CurrentX;
	int charposy = CurrentY;

	recti rc(charposx, charposy, charposx + m_iFontWidth, charposy + m_iFontHeight);

	if (!rc.isEmpty())			//如空格字符的rect为空，不需要绘制
	{
		//CLock lock(&g_Globals.textureCS);

		ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(tex, false);

		uint32_t pitch;
		uint8_t* data = (uint8_t*)texWriter->lock(0, pitch);
		ASSERT(data);

		//写纹理 A8L8
		if (tex->getColorFormat() == ECF_A8L8)
		{
			uint8_t* p = data + (charposy * pitch) + sizeof(uint16_t) * charposx; 

			for (int i = 0; i < m_iFontHeight; ++i)
			{
				const uint8_t *src = bitmapGlyph->bitmap.buffer + (i * bitmapGlyph->bitmap.pitch);       
				uint8_t *dst = reinterpret_cast<uint8_t*>(p);
				for (int j = 0; j < m_iFontWidth; ++j)
				{
					bool bEdge = (j >= bmpW || i >= bmpH);
					if (bEdge)
					{
						uint8_t alpha = 0;

						*dst++ = 0;		//color
						*dst++ = 0;		//alpha
					}
					else
					{
						uint8_t alpha = *src++;

						*dst++ = alpha; 	//color
						*dst++ = alpha;		//alpha
					}
				}
				p += pitch;
			}
		}
		else if (tex->getColorFormat() == ECF_A8R8G8B8)			//dx11 format
		{
			uint8_t* p = data + (charposy * pitch) + sizeof(uint32_t) * charposx;

			for (int i = 0; i < m_iFontHeight; ++i)
			{
				const uint8_t *src = bitmapGlyph->bitmap.buffer + (i * bitmapGlyph->bitmap.pitch);       
				uint8_t *dst = reinterpret_cast<uint8_t*>(p);  
				for (int j = 0; j < m_iFontWidth; ++j)
				{
					bool bEdge = (j >= bmpW || i >= bmpH);
					if (bEdge)
					{
						uint8_t alpha = 0;

						*dst++ = alpha; 	//b
						*dst++ = alpha; 	//g
						*dst++ = alpha; 	//r

						*dst++ = alpha;		//alpha
					}
					else
					{
						uint8_t alpha = *src++;

						*dst++ = alpha; 	//b
						*dst++ = alpha; 	//g
						*dst++ = alpha; 	//r

						*dst++ = alpha;		//alpha
					}
				}
				p += pitch;
			}
		}
		else
		{
			ASSERT(false);
		}

		texWriter->unlock(0);

		texWriter->copyToTexture(tex, &rc);
	}

	//添加信息
	SCharInfo charInfo;

	charInfo.TexIndex = (int)FontTextures.size()-1;
	charInfo.offsetX = offsetX;
	charInfo.offsetY = offsetY;
	charInfo.width = chW;
	charInfo.height = chH;
	charInfo.UVRect = rc;

	CharacterMap[ch] = charInfo;

	CurrentX = NextX;

	if (MaxY > NextY)
		NextY = MaxY;	

	return &CharacterMap[ch];
}

bool CFTFont::addFontTexture()
{
	E_DRIVER_TYPE type = g_Engine->getDriver()->getDriverType();
	ECOLOR_FORMAT format;

	if (type == EDT_DIRECT3D9 || type == EDT_OPENGL)
	{
		format = ECF_A8L8;
	}
	else
	{
		format = ECF_A8R8G8B8;
	}

	ITexture* fontTex = g_Engine->getManualTextureServices()->createEmptyTexture(dimension2du(FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE), format);	   //no mipmap
	FontTextures.push_back(fontTex);

	return true;
}

void CFTFont::drawA(const char* utf8text, SColor color, vector2di position, int nCharCount /*= -1*/, recti* pClip /*= nullptr*/)
{
	int x = position.X;
	int y = position.Y;

	//clip
	if (pClip &&
		(y >= pClip->bottom() || y + m_iFontHeight <= pClip->top() || x >= pClip->right()))
		return;

	S2DBlendParam blendParam = S2DBlendParam::UIFontBlendParam();
	float fInv = 1.0f / FONT_TEXTURE_SIZE;

	const char* p = utf8text;
	int nLen = nCharCount > 0 ? nCharCount : (int)strlen(utf8text);
	while(*p && nLen >= 0)
	{
		char16_t ch;
		int nadv;
		ch = (char16_t)Q_ParseUnicodeFromUTF8Str(p, &nadv, nLen);
		if(nadv == 0)		//parse end
			break;
		p += nadv;
		nLen -= nadv;
		if (ch == 0)		//string end
			break;

		const SCharInfo* charInfo = addChar(ch);

		if (!charInfo)
			continue;

		if ( ch == (char16_t)'\r' || ch == (char16_t)'\n') // Unix breaks
		{
			y += m_iFontHeight;
			x = position.X;
			continue;
		}

		int idx = charInfo->TexIndex;
		ASSERT(idx >= 0 && idx < (int)FontTextures.size());

		ITexture* texture = FontTextures[idx];

		SVertex_PCT verts[4];			//left top, right top, left bottom, right bottom

		float posX0 = (float)(x + charInfo->offsetX);
		float posX1 = (float)(posX0 + charInfo->UVRect.getWidth());
		float posY0 = (float)(y + charInfo->offsetY);
		float posY1 = (float)(posY0 + charInfo->UVRect.getHeight());

		float texX0 = charInfo->UVRect.left() * fInv;
		float texX1 = charInfo->UVRect.right() * fInv;
		float texY0 = charInfo->UVRect.top() * fInv;
		float texY1 = charInfo->UVRect.bottom() * fInv;

		float fLeft = posX0;
		float fTop = posY0;
		float fRight = posX1;
		float fBottom = posY1;
		float tu1 = 0.0f;
		float tu2 = 0.0f;
		float tv1 = 0.0f;
		float tv2 = 0.0f;

		if (pClip)			//clip
		{
			if (posX1 <= pClip->left() || posX0 >= pClip->right())
				continue;

			if (posX0 < pClip->left())
			{
				fLeft = (float)pClip->left();
				tu1 = (pClip->left() - posX0) * fInv;
			}

			if (posX1 > pClip->right())
			{
				fRight = (float)pClip->right();
				tu2 = (posX1 - pClip->right()) * fInv;
			}

			if (posY0 < pClip->top())
			{
				fTop = (float)pClip->top();
				tv1 = (pClip->top() - posY0) * fInv;
			}

			if (posY1 > pClip->bottom())
			{
				fBottom = (float)pClip->bottom();
				tv2 = (posY1 - pClip->bottom()) * fInv;
			}
		}

		float du = texX1 - texX0;
		float dv = texY1 - texY0;

		verts[0].Pos.set(fLeft, fTop, 0);
		verts[0].Color = color;
		verts[0].TCoords.set(
			texX0 + tu1 * du,
			texY0 + tv1 * dv);

		verts[1].Pos.set(fRight, fTop, 0);
		verts[1].Color = color;
		verts[1].TCoords.set(
			texX1 - tu2 * du,
			texY0 + tv1 * dv);

		verts[2].Pos.set(fLeft, fBottom, 0);
		verts[2].Color = color;
		verts[2].TCoords.set(
			texX0 + tu1 * du, 
			texY1 - tv2 * dv);

		verts[3].Pos.set(fRight, fBottom, 0);
		verts[3].Color = color;
		verts[3].TCoords.set(
			texX1 - tu2 * du, 
			texY1 - tv2 * dv);

		g_Engine->getDrawServices()->add2DQuads( texture,
			&verts[0],
			1,
			blendParam);

		x += charInfo->width;
	}

	g_Engine->getDrawServices()->flushAll2DQuads();
}

void CFTFont::drawW(const char16_t* text, SColor color, vector2di position, int nCharCount /*= -1*/, recti* pClip /*= nullptr*/)
{
	int x = position.X;
	int y = position.Y;

	//clip
	if (pClip &&
		(y >= pClip->bottom() || y + m_iFontHeight <= pClip->top() || x >= pClip->right()))
		return;

	S2DBlendParam blendParam = S2DBlendParam::UIFontBlendParam();
	float fInv = 1.0f / FONT_TEXTURE_SIZE;

	uint32_t len = nCharCount > 0 ? (uint32_t)nCharCount : (uint32_t)Q_UTF16Len(text);
	for (uint32_t i=0; i<len; ++i)
	{
		char16_t c = text[i];
		if (c == 0)		//string end
			continue;

		const SCharInfo* charInfo = addChar(c);

		if (!charInfo)
			continue;

		if ( c == (char16_t)'\r' || c == (char16_t)'\n') // Unix breaks
		{
			y += m_iFontHeight;
			x = position.X;
			continue;
		}

		int idx = charInfo->TexIndex;
		ASSERT(idx >= 0 && idx < (int)FontTextures.size());

		ITexture* texture = FontTextures[idx];

		SVertex_PCT verts[4];			//left top, right top, left bottom, right bottom

		float posX0 = (float)(x + charInfo->offsetX);
		float posX1 = (float)(posX0 + charInfo->UVRect.getWidth());
		float posY0 = (float)(y + charInfo->offsetY);
		float posY1 = (float)(posY0 + charInfo->UVRect.getHeight());

		float texX0 = charInfo->UVRect.left() * fInv;
		float texX1 = charInfo->UVRect.right() * fInv;
		float texY0 = charInfo->UVRect.top() * fInv;
		float texY1 = charInfo->UVRect.bottom() * fInv;

		float fLeft = posX0;
		float fTop = posY0;
		float fRight = posX1;
		float fBottom = posY1;
		float tu1 = 0.0f;
		float tu2 = 0.0f;
		float tv1 = 0.0f;
		float tv2 = 0.0f;

		if (pClip)			//clip
		{
			if (posX1 <= pClip->left() || posX0 >= pClip->right())
				continue;

			if (posX0 < pClip->left())
			{
				fLeft = (float)pClip->left();
				tu1 = (pClip->left() - posX0) * fInv;
			}

			if (posX1 > pClip->right())
			{
				fRight = (float)pClip->right();
				tu2 = (posX1 - pClip->right()) * fInv;
			}

			if (posY0 < pClip->top())
			{
				fTop = (float)pClip->top();
				tv1 = (pClip->top() - posY0) * fInv;
			}

			if (posY1 > pClip->bottom())
			{
				fBottom = (float)pClip->bottom();
				tv2 = (posY1 - pClip->bottom()) * fInv;
			}
		}

		float du = texX1 - texX0;
		float dv = texY1 - texY0;

		verts[0].Pos.set(fLeft, fTop, 0);
		verts[0].Color = color;
		verts[0].TCoords.set(
			texX0 + tu1 * du,
			texY0 + tv1 * dv);

		verts[1].Pos.set(fRight, fTop, 0);
		verts[1].Color = color;
		verts[1].TCoords.set(
			texX1 - tu2 * du,
			texY0 + tv1 * dv);

		verts[2].Pos.set(fLeft, fBottom, 0);
		verts[2].Color = color;
		verts[2].TCoords.set(
			texX0 + tu1 * du, 
			texY1 - tv2 * dv);

		verts[3].Pos.set(fRight, fBottom, 0);
		verts[3].Color = color;
		verts[3].TCoords.set(
			texX1 - tu2 * du, 
			texY1 - tv2 * dv);

		g_Engine->getDrawServices()->add2DQuads( texture,
			&verts[0],
			1,
			blendParam);

		x += charInfo->width;
	}

	g_Engine->getDrawServices()->flushAll2DQuads();
}

void CFTFont::addTextA( const char* utf8text, SColor color, vector2di position, int nCharCount, recti* pClip, bool bVertical)
{
	if (nCharCount == 0)			//-1表示实际长度，0或以上是指定长度
		return;

	uint32_t offset = (uint32_t)Texts.size();
	const char* p = utf8text;
	int nLen = nCharCount > 0 ? nCharCount : (int)strlen(utf8text);
	uint32_t uCharCount = 0;
	while(*p && nLen >= 0)
	{
		char16_t ch;
		int nadv;
		ch = (char16_t)Q_ParseUnicodeFromUTF8Str(p, &nadv, nLen);
		if(nadv == 0)		//parse end
			break;
		p += nadv;
		nLen -= nadv;
		if (ch == 0)		//string end
			break;

		Texts.push_back(ch);
		++uCharCount;
	}

	SDrawText d;
	d.offset = offset;
	d.length = uCharCount;
	d.color = color;
	d.position = position;
	d.bVertical = bVertical;
	d.bClip = pClip != nullptr;
	if (pClip)
		d.rcClip = *pClip;
	DrawTexts.push_back(d);
}

void CFTFont::addTextW( const char16_t* text, SColor color, vector2di position, int nCharCount, recti* pClip, bool bVertical )
{
	if (nCharCount == 0)		//-1表示实际长度，0或以上是指定长度
		return;

	uint32_t offset = (uint32_t)Texts.size();
	uint32_t uCharCount = nCharCount > 0  ? (uint32_t)nCharCount : (uint32_t)Q_UTF16Len(text);

	for (uint32_t i=0; i<uCharCount; ++i)
		Texts.push_back(text[i]);

	SDrawText d;
	d.offset = offset;
	d.length = uCharCount;
	d.color = color;
	d.position = position;
	d.bVertical = bVertical;
	d.bClip = pClip != nullptr;
	if (pClip)
		d.rcClip = *pClip;
	DrawTexts.push_back(d);
}

void CFTFont::flushText()
{
	drawTextWBatch();

	DrawTexts.clear();
	Texts.clear();
}

const IFTFont::SCharInfo* CFTFont::getCharInfo( char16_t ch ) const
{
	T_CharacterMap::const_iterator itr = CharacterMap.find(ch);
	if(itr != CharacterMap.end())
		return &itr->second;
	return nullptr;
}

int CFTFont::GetUnicodeCMapIndex( FT_Face ftFace )
{
	for (int i = 0; i < ftFace->num_charmaps; i++)
	{
		if (FT_ENCODING_UNICODE == ftFace->charmaps[i]->encoding)
			return i;
	}
	return -1;
}

FT_Face CFTFont::GetFTFace()
{
	FT_Face ftFace = nullptr;
	FTC_Manager_LookupFace(FontServices->GetCacheManager(), (FTC_FaceID)MyFaceID, &ftFace);
	return ftFace;
}

FT_Size CFTFont::GetFTSize()
{
	FT_Size ftSize = nullptr;
	FTC_Manager_LookupSize(FontServices->GetCacheManager(), FTCScaler, &ftSize);
	return ftSize;
}

FT_BitmapGlyph CFTFont::getCharExtent( uint32_t ch, int& offsetX, int& offsetY, int& chWidth, int& chHeight, int& chBmpW, int& chBmpH )
{
	FT_BitmapGlyph ftGlyph;

	ftGlyph = RenderChar(ch);

	if (!ftGlyph)
		return nullptr;

	FT_Size ftSize = GetFTSize();

	offsetX = ftGlyph->left;
	offsetY = (((ftSize->metrics.ascender << 1) + BoldWidth) >> 7) - ftGlyph->top;

	chWidth = FixedToInt(ftGlyph->root.advance.x) + (BoldWidth >> 6);
	chHeight = m_iFontHeight;

	chBmpW = ftGlyph->bitmap.width;
	chBmpH = ftGlyph->bitmap.rows;

	return ftGlyph;
}

FT_BitmapGlyph CFTFont::RenderChar( uint32_t ch )
{
	FTC_CMapCache mapCache = FontServices->GetCMapCache();
	FTC_FaceID ftFaceID = (FTC_FaceID)GetMyFaceID();

	//在cmap中找
	uint32_t index = FTC_CMapCache_Lookup(mapCache, ftFaceID, UnicodeCMapIndex, (uint32_t)ch);
	if (0 == index)
	{
		index = FTC_CMapCache_Lookup(mapCache, ftFaceID, UnicodeCMapIndex, L'□');
		if (0 == index)
			return nullptr;
	}

	return MyFTGlyphCache->LookupGlyph(index);
}

void CFTFont::drawTextWBatch()
{
	S2DBlendParam blendParam = S2DBlendParam::UIFontBlendParam();
	float fInv = 1.0f / FONT_TEXTURE_SIZE;

	for (uint32_t dt=0; dt<(uint32_t)DrawTexts.size(); ++dt)
	{
		const SDrawText& d = DrawTexts[dt];
		const char16_t* txt = &Texts[d.offset];

		if(!d.bVertical)
			drawText(d, txt, fInv, blendParam);
		else
			drawTextVertical(d, txt, fInv, blendParam);
	}

	g_Engine->getDrawServices()->flushAll2DQuads();
}

void CFTFont::drawText(const SDrawText& d, const char16_t* txt, float fInv, const S2DBlendParam& blendParam)
{
	bool bClip = d.bClip;
	const recti& rcClip = d.rcClip;

	int x = d.position.X;
	int y = d.position.Y;

	//clip
	if (bClip &&
		(y >= rcClip.bottom() || y + m_iFontHeight <= rcClip.top() || x >= rcClip.right()))
		return;

	uint32_t len = d.length;
	for (uint32_t i=0; i<len; ++i)
	{
		char16_t c = txt[i];
		if (c == (char16_t)0)
			break;

		const SCharInfo* charInfo = addChar(c);

		if (!charInfo)
			continue;

		if (c == (char16_t)'\r' || c == (char16_t)'\n') // Unix breaks
		{
			y += m_iFontHeight;
			x = d.position.X;
			continue;
		}

		int idx = charInfo->TexIndex;
		ASSERT(idx >= 0 && idx < (int)FontTextures.size());

		ITexture* texture = FontTextures[idx];

		SVertex_PCT verts[4];			//left top, right top, left bottom, right bottom

		int W =  charInfo->UVRect.getWidth();
		int H =  charInfo->UVRect.getHeight();

		float posX0 = (float)(x + charInfo->offsetX);
		float posX1 = (float)(posX0 + W);
		float posY0 = (float)(y + charInfo->offsetY);
		float posY1 = (float)(posY0 + H);

		float texX0 = charInfo->UVRect.left() * fInv;
		float texX1 = charInfo->UVRect.right() * fInv;
		float texY0 = charInfo->UVRect.top() * fInv;
		float texY1 = charInfo->UVRect.bottom() * fInv;

		float fLeft = posX0;
		float fTop = posY0;
		float fRight = posX1;
		float fBottom = posY1;
		float tu1 = 0.0f;
		float tu2 = 0.0f;
		float tv1 = 0.0f;
		float tv2 = 0.0f;

		if (bClip)			//clip
		{
			if (posX1 <= rcClip.left() || posX0 >= rcClip.right())
				continue;

			if (posX0 < rcClip.left())
			{
				fLeft = (float)rcClip.left();
				tu1 = (rcClip.left() - posX0) / W;
			}

			if (posX1 > rcClip.right())
			{
				fRight = (float)rcClip.right();
				tu2 = (posX1 - rcClip.right()) / W;
			}

			if (posY0 < rcClip.top())
			{
				fTop = (float)rcClip.top();
				tv1 = (rcClip.top() - posY0) / H;
			}

			if (posY1 > rcClip.bottom())
			{
				fBottom = (float)rcClip.bottom();
				tv2 = (posY1 - rcClip.bottom()) / H;
			}
		}

		float du = texX1 - texX0;
		float dv = texY1 - texY0;

		verts[0].Pos.set(fLeft, fTop, 0);
		verts[0].Color = d.color;
		verts[0].TCoords.set(
			texX0 + tu1 * du,
			texY0 + tv1 * dv);

		verts[1].Pos.set(fRight, fTop, 0);
		verts[1].Color = d.color;
		verts[1].TCoords.set(
			texX1 - tu2 * du,
			texY0 + tv1 * dv);

		verts[2].Pos.set(fLeft, fBottom, 0);
		verts[2].Color = d.color;
		verts[2].TCoords.set(
			texX0 + tu1 * du, 
			texY1 - tv2 * dv);

		verts[3].Pos.set(fRight, fBottom, 0);
		verts[3].Color = d.color;
		verts[3].TCoords.set(
			texX1 - tu2 * du, 
			texY1 - tv2 * dv);

		g_Engine->getDrawServices()->add2DQuads( texture,
			&verts[0],
			1,
			blendParam);

		x += charInfo->width;
	}
}

void CFTFont::drawTextVertical(const SDrawText& d, const char16_t* txt, float fInv, const S2DBlendParam& blendParam)
{
	bool bClip = d.bClip;
	const recti& rcClip = d.rcClip;

	int x = d.position.X;
	int y = d.position.Y;

	//clip
	if (bClip &&
		(y >= rcClip.bottom() || y + m_iFontHeight <= rcClip.top() || x >= rcClip.right()))
		return;

	uint32_t len = d.length;
	for (uint32_t i=0; i<len; ++i)
	{
		char16_t c = txt[i];
		if (c == (char16_t)0)
			break;

		const SCharInfo* charInfo = addChar(c);

		if (!charInfo)
			continue;

		if (c == (char16_t)'\r' || c == (char16_t)'\n') // Unix breaks
		{
			y += m_iFontHeight;
			x = d.position.X;
			continue;
		}

		int idx = charInfo->TexIndex;
		ASSERT(idx >= 0 && idx < (int)FontTextures.size());

		ITexture* texture = FontTextures[idx];

		SVertex_PCT verts[4];			//left top, right top, left bottom, right bottom

		int W =  charInfo->UVRect.getWidth();
		int H =  charInfo->UVRect.getHeight();

		float posX0 = (float)(x + charInfo->offsetX);
		float posX1 = (float)(posX0 + W);
		float posY0 = (float)(y + charInfo->offsetY);
		float posY1 = (float)(posY0 + H);

		float texX0 = charInfo->UVRect.left() * fInv;
		float texX1 = charInfo->UVRect.right() * fInv;
		float texY0 = charInfo->UVRect.top() * fInv;
		float texY1 = charInfo->UVRect.bottom() * fInv;

		float fLeft = posX0;
		float fTop = posY0;
		float fRight = posX1;
		float fBottom = posY1;
		float tu1 = 0.0f;
		float tu2 = 0.0f;
		float tv1 = 0.0f;
		float tv2 = 0.0f;

		if (bClip)			//clip
		{
			if (posX1 <= rcClip.left() || posX0 >= rcClip.right())
				continue;

			if (posX0 < rcClip.left())
			{
				fLeft = (float)rcClip.left();
				tu1 = (rcClip.left() - posX0) / W;
			}

			if (posX1 > rcClip.right())
			{
				fRight = (float)rcClip.right();
				tu2 = (posX1 - rcClip.right()) / W;
			}

			if (posY0 < rcClip.top())
			{
				fTop = (float)rcClip.top();
				tv1 = (rcClip.top() - posY0) / H;
			}

			if (posY1 > rcClip.bottom())
			{
				fBottom = (float)rcClip.bottom();
				tv2 = (posY1 - rcClip.bottom()) / H;
			}
		}

		float du = texX1 - texX0;
		float dv = texY1 - texY0;

		verts[0].Pos.set(fLeft, fTop, 0);
		verts[0].Color = d.color;
		verts[0].TCoords.set(
			texX0 + tu1 * du,
			texY0 + tv1 * dv);

		verts[1].Pos.set(fRight, fTop, 0);
		verts[1].Color = d.color;
		verts[1].TCoords.set(
			texX1 - tu2 * du,
			texY0 + tv1 * dv);

		verts[2].Pos.set(fLeft, fBottom, 0);
		verts[2].Color = d.color;
		verts[2].TCoords.set(
			texX0 + tu1 * du, 
			texY1 - tv2 * dv);

		verts[3].Pos.set(fRight, fBottom, 0);
		verts[3].Color = d.color;
		verts[3].TCoords.set(
			texX1 - tu2 * du, 
			texY1 - tv2 * dv);

		g_Engine->getDrawServices()->add2DQuads( texture,
			&verts[0],
			1,
			blendParam);

		y += m_iFontHeight;
	}
}
