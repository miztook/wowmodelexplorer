#pragma once

#include "base.h"
#include "dimension2d.h"
#include "fontDef.h"
#include "function.h"
#include <unordered_map>

class IFTFont;

//管理使用的所有字体
class IFontServices
{
public:
	explicit IFontServices(u32 defaultFontSize) { DefaultFontSize = defaultFontSize; }
	virtual ~IFontServices() {}

public:
	struct SFontKey
	{
		SFontKey(u32 _size, int _style, int _outlineWidth)
			: size(_size), style(_style), outlineWidth(_outlineWidth) {}

		u32 size;
		int style;
		int outlineWidth;

		bool operator<(const SFontKey& other) const
		{
			if(size != other.size)
				return size < other.size;
			else if (style != other.style)
				return style < other.style;
			else if (outlineWidth < other.outlineWidth)
				return outlineWidth < other.outlineWidth;
			return false;
		}

		bool operator==(const SFontKey& other) const
		{
			return size == other.size &&
				style == other.style &&
				outlineWidth == other.outlineWidth;
		}
	};

	struct SFontKey_hash
	{
		size_t operator()(const SFontKey& _Keyval) const
		{
			return (size_t)CRC32_BlockChecksum(&_Keyval, sizeof(_Keyval));
		}
	};

public:
	IFTFont* getDefaultFont();
	IFTFont* getFont(u32 fontSize, int fontStyle, int outlineWidth);
	IFTFont* getFont(SFontKey type);

public:
	virtual IFTFont* createFont(SFontKey type) = 0;
	virtual void destroyFont(SFontKey type) = 0;
	virtual void onWindowSizeChanged(const dimension2du& size) = 0;
	virtual void flushAllText() = 0;
	virtual bool createDefaultFonts() = 0;

protected:
	typedef std::unordered_map<SFontKey, IFTFont*, SFontKey_hash>	T_FontMap;

	T_FontMap		FontMap;
	u32		DefaultFontSize;
};

inline IFTFont* IFontServices::getFont( SFontKey type )
{
	T_FontMap::iterator itr = FontMap.find(type);
	if (itr != FontMap.end())
		return itr->second;

	return createFont(type);
}

inline IFTFont* IFontServices::getFont( u32 fontSize, int fontStyle, int outlineWidth )
{
	return getFont(SFontKey(fontSize, fontStyle, outlineWidth));
}

inline IFTFont* IFontServices::getDefaultFont()
{
	return getFont(SFontKey(DefaultFontSize, STYLE_NORMAL, 0));
}
