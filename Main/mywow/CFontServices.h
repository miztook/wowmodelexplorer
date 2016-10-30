#pragma once

#include "IFontServices.h"
#include "fixstring.h"
#include <unordered_map>
#include <map>
#include "fontDef.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_RENDER_H

//在手游中用一种字体，减少复杂度
#define A_FACE_NAME		"Fonts/ARKai_C.ttf"
#define A_FACE_INDEX		0

class CFontServices : public IFontServices
{
public:
	CFontServices(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes, u32 nDefaultFontSize);
	~CFontServices();

public:
	virtual IFTFont* createFont(SFontKey type);
	virtual void destroyFont(SFontKey type);
	virtual void onWindowSizeChanged(const dimension2du& size);
	virtual void flushAllText();
	virtual bool createDefaultFonts();

	const FT_Library&	 getFTLibrary() const { return FTLibrary; }
	const FTC_Manager& GetCacheManager() const { return FTCManager; }
	const FTC_CMapCache& GetCMapCache() const { return FTCCMapCache; }
	const FTC_ImageCache& GetImageCache() const { return FTCImageCache; }
	int GetCacheSizeMax() { return CacheSizeMax; }

	My_FaceID LookupFaceID(const char *strFontPath, int nFaceIndex, int nFontStyle);
	void RemoveFaceID(My_FaceID faceId);

	u32 getWidestChar() const { return WidestChar; }

private:
	bool init(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes);
	void release();

	void calcTotalFontTextureNum();

public:
	struct SFontDesc 
	{
		string256 faceName;
		int faceIndex;
		u32 fontSize;
		int fontStyle;
		int outlineWidth;
	};

	struct FaceID_Key 
	{
		string256 file_path;
		int face_index;
		int font_style;

		bool operator<(const FaceID_Key& other) const
		{
			if (file_path != other.file_path)
				return file_path < other.file_path;

			if (face_index != other.face_index)
				return face_index < other.face_index;

			if (font_style != other.font_style)
				return font_style < other.font_style;

			return false;
		}

		bool operator==(const FaceID_Key& other) const
		{
			return file_path == other.file_path &&
				face_index == other.face_index &&
				font_style == font_style;
		}
	};

	struct FaceID_Key_hash
	{
		size_t operator()(const FaceID_Key& _Keyval) const
		{
			u32 uHash;
			a_CRC32_InitChecksum(uHash);

			string256 str(_Keyval.file_path);
			str.make_lower();
			HashString(uHash, str.c_str());

			Hash(uHash, _Keyval.face_index);

			Hash(uHash, _Keyval.font_style);

			a_CRC32_FinishChecksum(uHash);
			return size_t(uHash);
		}
	};

	struct FaceID_Node
	{
		My_FaceID faceId;
		int refCount;

		FaceID_Node(): faceId(NULL_PTR), refCount(0) {}
		FaceID_Node(My_FaceID faceId, int refCount): faceId(faceId), refCount(refCount) {}
	};

#ifdef USE_QALLOCATOR
	typedef std::map<FaceID_Key, FaceID_Node, std::less<FaceID_Key>, qzone_allocator<std::pair<FaceID_Key, FaceID_Node>>>	T_FaceIDMap;
#else
	typedef std::unordered_map<FaceID_Key, FaceID_Node, FaceID_Key_hash>	T_FaceIDMap;
#endif
	
private:
	u32		WidestChar;

	FT_Library FTLibrary;
	FTC_Manager FTCManager;
	FTC_CMapCache FTCCMapCache;
	FTC_ImageCache FTCImageCache;
	int CacheSizeMax;
	int TotalFontTextureNum;

	T_FaceIDMap		m_FaceIDMap;
};