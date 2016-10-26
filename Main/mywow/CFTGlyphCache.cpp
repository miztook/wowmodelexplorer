#include "stdafx.h"
#include "CFTGlyphCache.h"
#include "mywow.h"
#include "CFontServices.h"
#include "CFTFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_RENDER_H
#include FT_OUTLINE_H

CFTGlyphCacheNode::CFTGlyphCacheNode()
	: nIndex(-1), nSize(0), glyph(NULL_PTR)
{
	InitializeListHead(&Link);
}

CFTGlyphCacheNode::~CFTGlyphCacheNode()
{
	if(glyph)
		FT_Done_Glyph((FT_Glyph)glyph);
}

CFTGlyphCache::CFTGlyphCache( CFTFont* font )
	: m_FTFont(font)
{
	m_nNodeCount = 0;
	m_nNodeSizeSum = 0;

	InitializeListHead(&m_GlyphNodeList);
}

CFTGlyphCache::~CFTGlyphCache()
{
	for (PLENTRY e = m_GlyphNodeList.Flink; e != &m_GlyphNodeList; )
	{
		CFTGlyphCacheNode* node = reinterpret_cast<CFTGlyphCacheNode*>CONTAINING_RECORD(e, CFTGlyphCacheNode, Link);
		e = e->Flink;

		delete node;
	}
	InitializeListHead(&m_GlyphNodeList);
}

FT_BitmapGlyph CFTGlyphCache::LookupGlyph( int nIndex )
{
	CFTGlyphCacheNode *pNode = FindInCache(nIndex);
	if (!pNode)
		pNode = NewNode(nIndex);

	if (!pNode)
		return NULL_PTR;
	else
		return pNode->glyph;
}

void CFTGlyphCache::AddHead( CFTGlyphCacheNode* pNode )
{
	InsertHeadList(&m_GlyphNodeList, &pNode->Link);

	m_GlyphCacheMap[pNode->nIndex] = pNode;

	++m_nNodeCount;
	m_nNodeSizeSum += pNode->nSize;
}

void CFTGlyphCache::RemoveTail()
{
	if (IsListEmpty(&m_GlyphNodeList))
		return;

	PLENTRY e = RemoveTailList(&m_GlyphNodeList);
	CFTGlyphCacheNode* node = reinterpret_cast<CFTGlyphCacheNode*>CONTAINING_RECORD(e, CFTGlyphCacheNode, Link);

	--m_nNodeCount;
	m_nNodeSizeSum -= node->nSize;

	T_GlyphCacheMap::iterator itr = m_GlyphCacheMap.find(node->nIndex);
	if (itr != m_GlyphCacheMap.end())
	{
		m_GlyphCacheMap.erase(itr);
	}

	delete node;
}

CFTGlyphCacheNode * CFTGlyphCache::NewNode( int nIndex )
{
	CFontServices* fontServices = static_cast<CFontServices*>(g_Engine->getFontServices());

	FT_Glyph ftResultGlyph = NULL_PTR;
	FT_Error error = 0;

	// get outline
	FT_Size ftSize = m_FTFont->GetFTSize();
	FT_Face ftFace =  ftSize->face;

	// by Silas - using FreeType Glyph Cache.
	FT_Glyph rawGlyph;
	FTC_Node rawNode;
	FTC_ImageTypeRec imageDesc;
	imageDesc.face_id = m_FTFont->GetMyFaceID();
	imageDesc.width = ftSize->metrics.x_ppem;//0;
	imageDesc.height = ftSize->metrics.y_ppem;//TheFont->GetFTCScaler()->height >> 6;
	imageDesc.flags = FT_LOAD_NO_BITMAP | /*FT_LOAD_NO_HINTING |*/ FT_LOAD_NO_AUTOHINT;
	error = FTC_ImageCache_Lookup(fontServices->GetImageCache(), &imageDesc, nIndex, &rawGlyph, &rawNode);
	if(rawGlyph->format == FT_GLYPH_FORMAT_OUTLINE)
	{
		FT_Glyph_Copy(rawGlyph, &ftResultGlyph);

		if ((unsigned long)ftFace->generic.data & STYLE_BOLD)
		{
			FT_OutlineGlyph outlineGlyph = (FT_OutlineGlyph)ftResultGlyph;
			FT_Pos      xstr;
			xstr = m_FTFont->GetBoldWidth();
			FT_Outline_Embolden(&outlineGlyph->outline, xstr);
		}
		if ((unsigned long)ftFace->generic.data & STYLE_ITALIC)
		{
			FT_Matrix    transform;
			transform.xx = 0x10000L;
			transform.yx = 0x00000L;

			transform.xy = 0x0366AL;
			transform.yy = 0x10000L;

			FT_Glyph_Transform(ftResultGlyph, &transform, NULL_PTR);
		}
		error = FT_Glyph_To_Bitmap(&ftResultGlyph, FT_RENDER_MODE_NORMAL, NULL_PTR, 1);
	}
	FTC_Node_Unref(rawNode, fontServices->GetCacheManager());
	if(ftResultGlyph == NULL_PTR)
		return NULL_PTR;

	CFTGlyphCacheNode* pNode = new CFTGlyphCacheNode;
	pNode->nIndex = nIndex;
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph) ftResultGlyph;
	pNode->glyph = bitmapGlyph;
	pNode->nSize += 
		sizeof(bitmapGlyph) + 
		sizeof(bitmapGlyph->bitmap) + 
		bitmapGlyph->bitmap.rows * bitmapGlyph->bitmap.pitch;

	AddNode(pNode);

	return pNode;
}

void CFTGlyphCache::AddNode( CFTGlyphCacheNode* pNode )
{
	CFontServices* fontServices = static_cast<CFontServices*>(g_Engine->getFontServices());

	if (IsListEmpty(&m_GlyphNodeList) &&
		m_nNodeSizeSum + pNode->nSize > fontServices->GetCacheSizeMax())			//full
	{
		RemoveTail();
	}

	AddHead(pNode);
}

CFTGlyphCacheNode * CFTGlyphCache::FindInCache( int nIndex )
{
	T_GlyphCacheMap::iterator itr = m_GlyphCacheMap.find(nIndex);
	if (itr == m_GlyphCacheMap.end())
		return NULL_PTR;

	CFTGlyphCacheNode* p = itr->second;
	if (p)
	{
		if (m_nNodeCount > 1)		//move to head
		{
			RemoveEntryList(&p->Link);
			InsertHeadList(&m_GlyphNodeList, &p->Link);
		}
	}

	return p;
}


