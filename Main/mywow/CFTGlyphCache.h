#pragma once

#include "fontDef.h"
#include "linklist.h"
#include <unordered_map>
#include <map>

#include <ft2build.h>
#include FT_GLYPH_H

class CFTFont;

typedef struct FT_BitmapGlyphRec_*  FT_BitmapGlyph;

//维护freetype画glyph的一个缓存,

class CFTGlyphCacheNode
{
	DISALLOW_COPY_AND_ASSIGN(CFTGlyphCacheNode);

public:
	CFTGlyphCacheNode();
	~CFTGlyphCacheNode();

public:
	LENTRY		Link;

	int nIndex;
	FT_BitmapGlyph glyph;
	int nSize;
};

class CFTGlyphCache
{
public:
	explicit CFTGlyphCache(CFTFont* font);
	virtual ~CFTGlyphCache();

public:
	FT_BitmapGlyph LookupGlyph(int nIndex);

private:
	void AddHead(CFTGlyphCacheNode* pNode);
	void RemoveTail();
	CFTGlyphCacheNode * NewNode(int nIndex);
	CFTGlyphCacheNode * FindInCache(int nIndex);
	void AddNode(CFTGlyphCacheNode* pNode);

private:
#ifdef USE_QALLOCATOR
	typedef std::map<int, CFTGlyphCacheNode*, std::less<int>, qzone_allocator<std::pair<int, CFTGlyphCacheNode*>>>	T_GlyphCacheMap;
#else
	typedef std::unordered_map<int, CFTGlyphCacheNode*>	T_GlyphCacheMap;
#endif

	T_GlyphCacheMap		m_GlyphCacheMap;
	LENTRY	m_GlyphNodeList;
	int		m_nNodeCount;
	int		m_nNodeSizeSum;
	CFTFont*		m_FTFont;

};