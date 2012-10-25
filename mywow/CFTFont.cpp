#include "stdafx.h"
#include "CFTFont.h"
#include "mywow.h"

#define		INTER_GLYPH_PAD_SPACE 2
#define		LINE_PADDING		2

CFTFont::CFTFont( const char* faceName, u32 size )
{
	DrawBatches.reserve(4);

	string256 facePath = g_Engine->getFileSystem()->getFontBaseDirectory();
	facePath.append(faceName);

	// init 
	FT_Error error = FT_Init_FreeType(&ftLibrary);
	_ASSERT(error == 0);

	// face, size

	error = FT_New_Face(ftLibrary, facePath.c_str(), 0, &ftFace);
	if (error)
	{
		facePath = g_Engine->getFileSystem()->getFontBaseDirectory();
		facePath.append("msyh.ttf");

		error = FT_New_Face(ftLibrary, facePath.c_str(), 0, &ftFace);
		_ASSERT(error == 0);
	}

	ftSize = (FT_F26Dot6)( size * (1<<6) );
	error = FT_Set_Char_Size( ftFace, ftSize, ftSize, 96, 96 );
	_ASSERT(error == 0);

	Ascender = ftFace->size->metrics.ascender   >> 6;
	Descender = ftFace->size->metrics.descender >> 6;   // // 基线到字符轮廓最低点的距离
	FontHeight   = ((ftFace->size->metrics.height >> 6) + Ascender + Descender) / 2;

	addFontTexture();

	NextX = NextY = CurrentX = CurrentY = MaxY = 0;
}

CFTFont::~CFTFont()
{
	for (u32 i=0; i<DrawBatches.size(); ++i)
		DrawBatches[i].texture->drop();

	FT_Done_FreeType(ftLibrary);
}

void CFTFont::drawA( c8* text, SColor color, vector2di position, u32 glyphPadding, u32 linePadding )
{
	u32 len = min_((u32)strlen(text), 512u);
	str8to16(text, Tmp, 512 );

	drawW(Tmp, color, position, glyphPadding, linePadding);
}

void CFTFont::drawW( c16* text, SColor color, vector2di position, u32 glyphPadding, u32 linePadding )
{
	s32 x = position.X;
	s32 y = position.Y;

	u32 count = 0;
	s32 maxX = x;
	u32 len = min_((s32)wcslen(text), MAX_TEXT_LENGTH);
	for (u32 i=0; i<len; ++i)
	{
		c16 c = text[i];
		addChar(c);

		bool lineBreak=false;
		if ( c == L'\r') // Mac or Windows breaks
		{
			lineBreak = true;
			if ( text[i + 1] == L'\n') // Windows breaks
				c = text[++i];
		}
		else if ( c == L'\n') // Unix breaks
		{
			lineBreak = true;
		}

		if (lineBreak)
		{
			maxX = x;

			y += FontHeight + linePadding;
			x = position.X;
			continue;
		}

		int idx = CharacterMap[c].TexIndex;
		_ASSERT(idx >= 0 && idx < (s32)DrawBatches.size());

		s32 offsetx = CharacterMap[c].offsetX;
		s32 offsety = CharacterMap[c].offsetY;

		DrawBatches[idx].positions[count].set(x+offsetx, y+offsety);
		DrawBatches[idx].sourceRects[count] = &CharacterMap[c].UVRect;

		++count;

		x += max_(5, (s32)(CharacterMap[c].UVRect.getWidth()+glyphPadding));
	}

	for ( u32 i=0; i<DrawBatches.size(); ++i )
	{
		g_Engine->getDrawServices()->draw2DImageBatch( DrawBatches[i].texture,
			DrawBatches[i].positions,
			(const recti**)DrawBatches[i].sourceRects,
			count,
			1.0f,
			color,
			true);
	}
}

dimension2du CFTFont::getDimension( c16* text )
{
	s32 x = 0;
	s32 y = 0;

	u32 count = 0;
	s32 maxX = x;
	u32 len = min_((s32)wcslen(text), 512);
	for (u32 i=0; i<len; ++i)
	{
		c16 c = text[i];
		addChar(c);

		bool lineBreak=false;
		if ( c == L'\r') // Mac or Windows breaks
		{
			lineBreak = true;
			if ( text[i + 1] == L'\n') // Windows breaks
				c = text[++i];
		}
		else if ( c == L'\n') // Unix breaks
		{
			lineBreak = true;
		}

		if (lineBreak)
		{
			maxX = x;

			y += FontHeight + LINE_PADDING;
			x = 0;
			continue;
		}

		int idx = CharacterMap[c].TexIndex;
		_ASSERT(idx >= 0 && idx < (s32)DrawBatches.size());

		++count;

		x += max_(5, CharacterMap[c].UVRect.getWidth());
	}

	return dimension2du(x,y + FontHeight+LINE_PADDING);
}

ITexture* CFTFont::getTexture( u32 idx )
{
	if (idx >= DrawBatches.size())
		return NULL;

	return DrawBatches[idx].texture;
}

void CFTFont::addChar( c16 ch )
{
	if (CharacterMap[ch].TexIndex != -1)
	{
		_ASSERT(CharacterMap[ch].TexIndex >= 0 && 
			CharacterMap[ch].TexIndex <= (s32)DrawBatches.size());
		return;
	}

	FT_GlyphSlot slot = ftFace->glyph;

	FT_Error error = FT_Load_Char(ftFace, ch, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT);
	_ASSERT(error == 0);

	unsigned int glyph_w = slot->bitmap.width + INTER_GLYPH_PAD_SPACE;  

	unsigned int glyph_h = slot->bitmap.rows + INTER_GLYPH_PAD_SPACE;  

	s32 bitmap_left = 0; //slot->bitmap_left;   // 字符距离左边界的距离		autohint后不需要
	s32 bitmap_top = slot->bitmap_top; // 字符最高点距离基线的距离

	NextX = CurrentX + glyph_w;

	if (NextX > FONT_TEXTURE_SIZE)				//换行
	{
		CurrentX = 0;
		NextX = CurrentX + glyph_w;
		CurrentY = NextY;
	}

	MaxY = CurrentY + glyph_h;						//换页, 增加纹理 
	if (MaxY > FONT_TEXTURE_SIZE)
	{
		ITexture* tex = DrawBatches.back().texture;

		addFontTexture();

		CurrentX = CurrentY = NextY = 0;
		MaxY = 0;
	}

	ITexture* tex = DrawBatches.back().texture;

	//写纹理 A8L8
	u16* data = (u16*)tex->lock();
	u16* p = NULL;
	//
	FT_Bitmap* glyph_bitmap = &ftFace->glyph->bitmap;

	s32 charposx = CurrentX + bitmap_left;
	s32 charposy = CurrentY + Ascender - bitmap_top;

	p = data + (charposy * FONT_TEXTURE_SIZE) + charposx;  

	for (int i = 0; i < glyph_bitmap->rows; ++i)  
	{  
		u8 *src = glyph_bitmap->buffer + (i * glyph_bitmap->pitch);       
		u8 *dst = reinterpret_cast<u8*>(p);   
		for (int j = 0; j < glyph_bitmap->width; ++j)  
		{  
			u8 alpha = *src++;

			*dst++ = alpha; 	//color

			*dst++ = alpha;		//alpha
		}  
		p += FONT_TEXTURE_SIZE;  
	}  

	tex->unlock();

	//添加信息
	CharacterMap[ch].TexIndex = DrawBatches.size()-1;
	CharacterMap[ch].offsetX = bitmap_left;
	CharacterMap[ch].offsetY = Ascender - bitmap_top;
	CharacterMap[ch].UVRect = recti(charposx, charposy, 
		charposx + slot->bitmap.width, charposy+slot->bitmap.rows);


	CurrentX = NextX;

	if (MaxY > NextY)
		NextY = MaxY;	
}

bool CFTFont::addFontTexture()
{
	ITexture* fontTex = g_Engine->getManualTextureServices()->createEmptyTexture(dimension2du(FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE), ECF_A8L8);	   //no mipmap

	DrawBatches.push_back(SDrawBatch());
	DrawBatches.back().texture = fontTex;

	return true;
}