#include "stdafx.h"
#include "CFontServices.h"
#include "mywow.h"
#include "CFTFont.h"

FT_Error MyFTCFaceRequest(FTC_FaceID  face_id,
	FT_Library  library,
	FT_Pointer  request_data,
	FT_Face*    aface )
{
	My_FaceID myFaceID = (My_FaceID) face_id;

	FT_Error error = 0;

	char strFullFontPath[MAX_PATH];
	Q_strcpy(strFullFontPath, MAX_PATH, myFaceID->file_path);

	error = FT_New_Face(
		library, 
		strFullFontPath, 
		myFaceID->face_index, 
		aface);

	if (!error)
		(*aface)->generic.data = (void *)myFaceID->font_style;
	return error;
}

CFontServices::CFontServices(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes, u32 nDefaultFontSize)
	: IFontServices(nDefaultFontSize)
{
	WidestChar = L'W';
	TotalFontTextureNum = 0;

	// init
	if (nMaxFontFamilyCountInCache == 0) 
		nMaxFontFamilyCountInCache = 4;
	if (nMaxFontStyleInCache == 0) 
		nMaxFontStyleInCache = 8;
	if (nMaxCacheSizeBytes == 0) 
		nMaxCacheSizeBytes = 1024 * 1024 * 2;

	CacheSizeMax = nMaxCacheSizeBytes;

	init(nMaxFontFamilyCountInCache, nMaxFontStyleInCache, nMaxCacheSizeBytes);
}

CFontServices::~CFontServices()
{
	CSysUtility::outputDebug("Number of Fonts: %d", (int)FontMap.size());
	CSysUtility::outputDebug("Max Number of Font Textures: %d", TotalFontTextureNum);

	release();

	FTC_Manager_Done(FTCManager);

	FT_Done_FreeType(FTLibrary);
}

IFTFont* CFontServices::createFont( SFontKey type )
{
	T_FontMap::iterator itr = FontMap.find(type);
	if (itr != FontMap.end())
	{
		ASSERT(false);
		return NULL_PTR;
	}

	CFTFont* pFont = new CFTFont(A_FACE_NAME, 
		A_FACE_INDEX, 
		type.size, 
		type.style,
		type.outlineWidth); 

	if (!pFont->init())
	{
		delete pFont;
		CSysUtility::outputDebug("CFontServices::createFont failed! font face: %s", A_FACE_NAME);
		return NULL_PTR;
	}

	FontMap[type] = pFont;

	calcTotalFontTextureNum();

	return pFont;
}

void CFontServices::destroyFont( SFontKey type )
{
	T_FontMap::iterator itr = FontMap.find(type);
	if (itr != FontMap.end())
	{
		delete itr->second;
		FontMap.erase(itr);
	}
}

bool CFontServices::init(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes)
{
	FT_Error error;
	error = FT_Init_FreeType(&FTLibrary);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	error = FTC_Manager_New(FTLibrary, nMaxFontFamilyCountInCache, nMaxFontStyleInCache, nMaxCacheSizeBytes, MyFTCFaceRequest, NULL_PTR, &FTCManager);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	error = FTC_CMapCache_New(FTCManager, &FTCCMapCache);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	error = FTC_ImageCache_New(FTCManager, &FTCImageCache);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void CFontServices::release()
{
	for (T_FontMap::iterator itr = FontMap.begin(); itr != FontMap.end(); ++itr)
	{
		delete itr->second;
	}
	FontMap.clear();
}

void CFontServices::calcTotalFontTextureNum()
{
	int numTextures = 0;
	for (T_FontMap::iterator itr = FontMap.begin(); itr != FontMap.end(); ++itr)
	{
		CFTFont* ftFont = static_cast<CFTFont*>(itr->second);
		numTextures += (int)ftFont->getTextureCount();
	}

	if (numTextures > TotalFontTextureNum)
		TotalFontTextureNum = numTextures;
}

bool CFontServices::createDefaultFonts()
{
	IFTFont* pFont = createFont(SFontKey(DefaultFontSize, STYLE_NORMAL, 0));
	if (pFont)
		pFont->setFixedSize(true);
	return pFont != NULL_PTR;
}

void CFontServices::onWindowSizeChanged( const dimension2du& size )
{
	//清除font重新建立，这样可以删除不用的字体，FixedSize除外

	for (T_FontMap::iterator itr = FontMap.begin(); itr != FontMap.end();)
	{
		IFTFont* pFont = itr->second;
		if (!pFont->isFixedSize())
		{
			FontMap.erase(itr++);
			delete pFont;
		}
		else
		{
			++itr;
		}
	}
}

My_FaceID CFontServices::LookupFaceID( const char* strFontPath, int nFaceIndex, int nFontStyle )
{
	FaceID_Key key;
	key.file_path = strFontPath;
	key.face_index = nFaceIndex;
	key.font_style = nFontStyle;

	ASSERT(!isAbsoluteFileName(strFontPath));

	T_FaceIDMap::iterator itr = m_FaceIDMap.find(key);
	if (itr != m_FaceIDMap.end())
	{
		FaceID_Node& node = itr->second;
		++node.refCount;
		return node.faceId;
	}

	My_FaceID faceId = new My_FaceID_Rec;
	strcpy(faceId->file_path, strFontPath);
	faceId->face_index = nFaceIndex;
	faceId->font_style = nFontStyle;

	m_FaceIDMap[key] = FaceID_Node(faceId, 1);
	return faceId;
}

void CFontServices::RemoveFaceID( My_FaceID faceId )
{
	if (faceId == NULL_PTR)
		return;

	FaceID_Key key;
	key.file_path = faceId->file_path;
	key.face_index = faceId->face_index;
	key.font_style = faceId->font_style;

	T_FaceIDMap::iterator itr = m_FaceIDMap.find(key);
	if (itr != m_FaceIDMap.end())
	{
		FaceID_Node& node = itr->second;
		--node.refCount;

		if (node.refCount <= 0)
		{
			FTC_Manager_RemoveFaceID(FTCManager, (FTC_FaceID)node.faceId);
			delete node.faceId;
			m_FaceIDMap.erase(itr);
		}
	}
}

void CFontServices::flushAllText()
{
	for (T_FontMap::iterator itr = FontMap.begin(); itr != FontMap.end(); ++itr)
	{
		IFTFont* font = itr->second;
		font->flushText();
	}
}


