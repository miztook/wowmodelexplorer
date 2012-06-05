#pragma once

#include <crtdbg.h>

#include "qzone_allocator.h"

typedef		unsigned __int8		u8;
typedef		__int8		s8;

typedef		char		c8;
typedef		wchar_t	c16;

typedef		unsigned	__int16		u16;
typedef		__int16		s16;

typedef		unsigned	__int32		u32;
typedef		__int32		s32;

typedef		float		f32;
//	typedef	double	f64;

typedef		char		UTF8;
typedef		unsigned __int16		UTF16;
typedef		unsigned	__int32		UTF32;

#ifndef MAX_PATH
#define		MAX_PATH	260
#endif

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

#define		DEFAULT_SIZE	64
#define		MAX_TEXT_LENGTH	512
#define		FONT_TEXTURE_SIZE	512

#define VS11	"vs_1_1"
#define VS20	"vs_2_0"
#define VS2A	"vs_2_a"
#define VS30	"vs_3_0"

#define PS11	"ps_1_1"
#define PS12	"ps_1_2"
#define PS13	"ps_1_3"
#define PS14	"ps_1_4"
#define PS20	"ps_2_0"
#define PS2A	"ps_2_a"
#define PS2B	"ps_2_b"
#define PS30	"ps_3_0"


template <class T>
class IResourceCache;

template <class T>
class IReferenceCounted
{
public:
	//
	IReferenceCounted() : ReferenceCounter(1), Cache(NULL) {}
	virtual ~IReferenceCounted(){}

	//
	void grab()
	{ 
		++ReferenceCounter;
	}
	bool drop()
	{
		_ASSERT( ReferenceCounter>0 );
		--ReferenceCounter;
		if (ReferenceCounter == 1 && Cache)
		{
			onRemove();
			Cache->removeFromCache(static_cast<T*>(this));
		}
		else
		if ( !ReferenceCounter )
		{
			delete this;
			return true;
		}
		return false;
	}

	//
	s32 getReferenceCount() const { return ReferenceCounter; }
	const c8* getFileName() const { return fileName; }
	void setFileName(const c8* filename) { strcpy_s(fileName, MAX_PATH, filename); }

	IResourceCache<T>* Cache;	

protected:
	//对于有显存资源的类(ITexture, IFileM2等)，为了优化多线程加载和节省显存，加载线程只加载内存资源，主线程负责构造显存资源
	virtual void onRemove() = 0;

private:
	s32 ReferenceCounter;
	c8		fileName[MAX_PATH];
};

class ILostResetCallback
{
public:
	virtual void onLost() = 0;
	virtual void onReset() = 0;
};

//list

#define CONTAINING_RECORD(address, type, field) ((type *)( \
	(PCHAR)(address) - \
	(ULONG_PTR)(&((type *)0)->field)))


typedef struct _LENTRY {
	struct _LENTRY *Flink;
	struct _LENTRY *Blink;
} LENTRY, *PLENTRY;


inline void InitializeListHead(PLENTRY ListHead)
{
	ListHead->Flink = ListHead->Blink = ListHead;
}

#define IsListEmpty(ListHead) \
	((ListHead)->Flink == (ListHead))

inline bool RemoveEntryList(PLENTRY Entry)
{
	PLENTRY Blink;
	PLENTRY Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
	return (Flink == Blink);
}

inline PLENTRY RemoveHeadList(PLENTRY ListHead)
{
	PLENTRY Flink;
	PLENTRY Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;
	return Entry;
}


inline PLENTRY RemoveTailList(PLENTRY ListHead)
{
	PLENTRY Blink;
	PLENTRY Entry;

	Entry = ListHead->Blink;
	Blink = Entry->Blink;
	ListHead->Blink = Blink;
	Blink->Flink = ListHead;
	return Entry;
}


inline void InsertTailList(PLENTRY ListHead, PLENTRY Entry)
{
	PLENTRY Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}

inline void InsertHeadList( PLENTRY ListHead, PLENTRY Entry)
{
	PLENTRY Flink;

	Flink = ListHead->Flink;
	Entry->Flink = Flink;
	Entry->Blink = ListHead;
	Flink->Blink = Entry;
	ListHead->Flink = Entry;
}


enum E_DRIVER_TYPE
{
	EDT_NULL = 0,
	EDT_DIRECT3D9,
	EDT_DIRECT3D11,
	EDT_COUNT,

	EDT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_CSAA
{
	ECSAA_NONE = 0,
	ECSAA_8x,
	ECSAA_8xQ,
	ECSAA_16x,
	ECSAA_16xQ,

	ECSAA_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_TRANSFORMATION_STATE
{
	ETS_VIEW = 0,
	ETS_WORLD,
	ETS_PROJECTION,
	ETS_TEXTURE_0,
	ETS_TEXTURE_1,
	ETS_TEXTURE_2,
	ETS_TEXTURE_3,
	ETS_COUNT,

	ETS_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_VIDEO_DRIVER_FEATURE
{
	EVDF_RENDER_TO_TARGET = 0,
	EVDF_HARDWARE_TL,
	EVDF_TEXTURE_ADDRESS,
	EVDF_SEPARATE_UVWRAP,
	EVDF_MIP_MAP,
	EVDF_MIP_MAP_AUTO_UPDATE,
	EVDF_STENCIL_BUFFER,
	EVDF_VERTEX_SHADER_1_1,
	EVDF_VERTEX_SHADER_2_0,
	EVDF_VERTEX_SHADER_3_0,
	EVDF_VERTEX_SHADER_4_0,
	EVDF_VERTEX_SHADER_5_0,
	EVDF_PIXEL_SHADER_1_1,
	EVDF_PIXEL_SHADER_2_0,
	EVDF_PIXEL_SHADER_3_0,
	EVDF_PIXEL_SHADER_4_0,
	EVDF_PIXEL_SHADER_5_0,
	EVDF_GEOMETRY_SHADER_4_0,
	EVDF_GEOMETRY_SHADER_5_0,
	EVDF_HULL_SHADER_5_0,
	EVDF_TEXTURE_NSQUARE,
	EVDF_TEXTURE_NPOT,
	EVDF_COLOR_MASK,
	EVDF_MULTIPLE_RENDER_TARGETS,
	EVDF_MRT_COLOR_MASK,
	EVDF_MRT_BLEND_FUNC,
	EVDF_MRT_BLEND,
	EVDF_STREAM_OFFSET,
	EVDF_W_BUFFER,
	EVDF_COUNT,

	EVDF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum ECOLOR_FORMAT
{
	//16
//	ECF_A8 = 0,
	ECF_A8L8 = 0,

	ECF_A1R5G5B5,
	ECF_R5G6B5,

	//24
	ECF_R8G8B8,

	//32
	ECF_A8R8G8B8,

	ECF_UNKNOWN,

	ECF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

inline u32 getBitsPerPixelFromFormat(  ECOLOR_FORMAT format )
{
	switch(format)
	{
// 	case ECF_A8:
// 		return 8;
	case ECF_A8L8:
	case ECF_A1R5G5B5:
	case ECF_R5G6B5:
		return 16;
	case ECF_R8G8B8:
		return 24;
	case ECF_A8R8G8B8:
		return 32;
	default:
		return 0;
	}
}

enum E_SHADER_VERSION
{
	ESV_VS_1_1 = 0,
	ESV_VS_2_0,
	ESV_VS_2_a,
	ESV_VS_3_0,
	ESV_VS_4_0,
	ESV_VS_4_1,
	ESV_VS_5_0,

	ESV_PS_1_1,
	ESV_PS_1_2,
	ESV_PS_1_3,
	ESV_PS_1_4,
	ESV_PS_2_0,
	ESV_PS_2_a,
	ESV_PS_2_b,
	ESV_PS_3_0,
	ESV_PS_4_0,
	ESV_PS_4_1,
	ESV_PS_5_0,

	ESV_GS_4_0,

	ESV_COUNT,

	ESV_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_VERTEX_TYPE
{
	EVT_BASICCOLOR = 0,			//for bounding box
	EVT_BASICTEX,
	EVT_STANDARD,
	EVT_MODEL,						//fvf
	EVT_BONEINDICES,
	EVT_COUNT,

	EVT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_MESHBUFFER_MAPPING
{
	EMM_SOFTWARE = 0,					
	EMM_STATIC,
	EMM_DYNAMIC,
	EMM_COUNT,

	EMM_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_INDEX_TYPE
{
	EIT_16BIT = 0,
	EIT_32BIT,
	EIT_COUNT,

	EIT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_PRIMITIVE_TYPE
{
	EPT_POINTS=0,
	EPT_LINE_STRIP,
	EPT_LINES,
	EPT_TRIANGLE_STRIP,
	EPT_TRIANGLE_FAN,
	EPT_TRIANGLES,
	EPT_COUNT,

	EPT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

inline u32 getPrimitiveCount(E_PRIMITIVE_TYPE primType, u32 count)
{
	u32 p = 0;

	switch (primType)
	{
	case EPT_POINTS:
		p= count;
		break;
	case EPT_LINES:
		_ASSERT(count>=2);
		p = count/2;
		break;
	case EPT_LINE_STRIP:
		_ASSERT(count>=2);
		p = count - 1;
		break;
	case EPT_TRIANGLE_FAN:
		_ASSERT(count>=3);
		p = count - 2;
		break;
	case EPT_TRIANGLE_STRIP:
		_ASSERT(count>=3);
		p = count - 2;
		break;
	case EPT_TRIANGLES:
		_ASSERT(count>=3);
		p = count / 3;
		break;
	default:
		_ASSERT(false);
	}
	return p;
}

enum E_MATERIAL_TYPE
{
	EMT_2D = 0,
	EMT_ONECOLOR,			//no texture, for line	
	EMT_SOLID,

	EMT_LIGHTMAP,				//modulate
	EMT_LIGHTMAP_ADD,			//add
	EMT_LIGHTMAP_M2,			//modulate2
	EMT_LIGHTMAP_M4,			//modulate4
	EMT_LIGHTMAP_LIGHTING,
	EMT_LIGHTMAP_LIGHTING_M2,
	EMT_LIGHTMAP_LIGHTING_M4,
	EMT_DETAIL_MAP,

	EMT_ALPHA_TEST,
	EMT_TRANSPARENT_ADD_ALPHA,	
	EMT_TRANSPARENT_ALPHA_BLEND,
	EMT_TRANSPARENT_ADD_COLOR,
	
	EMT_COUNT,

	EMT_FORCE_32BIT = 0x7fffffff
};

enum E_COMPARISON_FUNC
{
	ECFN_NEVER = 0,
	ECFN_LESSEQUAL,
	ECFN_EQUAL,
	ECFN_LESS,
	ECFN_NOTEQUAL,
	ECFN_GREATEREQUAL,
	ECFN_GREATER,
	ECFN_ALWAYS,

	ECFN_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_BLEND_FACTOR
{
	EBF_ZERO = 0,
	EBF_ONE,
	EBF_DST_COLOR,
	EBF_ONE_MINUS_DST_COLOR,
	EBF_SRC_COLOR,
	EBF_ONE_MINUS_SRC_COLOR,
	EBF_SRC_ALPHA,
	EBF_ONE_MINUS_SRC_ALPHA,
	EBF_DST_ALPHA,
	EBF_ONE_MINUS_DST_ALPHA,
	EBF_SRC_ALPHA_SATURATE,

	EBF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_TEXTURE_OP
{
	ETO_DISABLE = 0,
	ETO_ARG1,
	ETO_ARG2,
	ETO_MODULATE,
	ETO_MODULATE_X2,
	ETO_MODULATE_X4,
	ETO_ADD,
	ETO_ADDSIGNED,

	ETO_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_TEXTURE_ARG
{
	ETA_CURRENT = 0,
	ETA_TEXTURE,
	ETA_DIFFUSE,

	ETA_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_TEXTURE_ADDRESS
{
	ETA_U = 0,
	ETA_V,
	ETA_W,
};

enum E_TEXTURE_CLAMP
{
	ETC_REPEAT = 0,
	ETC_CLAMP,
	ETC_CLAMP_TO_BORDER,
	ETC_MIRROR,
	ETC_MIRROR_CLAMP,
	ETC_MIRROR_CLAMP_TO_BORDER,
	ETC_COUNT,

	ETC_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_TEXTURE_FILTER
{
	ETF_NONE = 0,
	ETF_BILINEAR,
	ETF_TRILINEAR,
	ETF_ANISOTROPIC_X1,
	ETF_ANISOTROPIC_X2,
	ETF_ANISOTROPIC_X4,
	ETF_ANISOTROPIC_X8,


	ETF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

inline u8 getAnisotropic(E_TEXTURE_FILTER filter)
{
	switch(filter)
	{
	case ETF_ANISOTROPIC_X1:
		return 1;
	case ETF_ANISOTROPIC_X2:
		return 2;
	case ETF_ANISOTROPIC_X4:
		return 4;
	case ETF_ANISOTROPIC_X8:
		return 8;
	default:
		return 0;
	}
}


#define  MATERIAL_MAX_TEXTURES		2

enum E_LIGHT_TYPE
{
	ELT_POINT = 0,
	ELT_SPOT,
	ELT_DIRECTIONAL,
	ELT_AMBIENT,

	ELT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

//雾
enum E_FOG_TYPE
{
	EFT_FOG_EXP=0,
	EFT_FOG_LINEAR,
	EFT_FOG_EXP2,

	EFT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_GEOMETRY_TYPE
{
	EGT_CUBE = 0,
	EGT_SPHERE,
	EGT_SKYDOME,
	EGT_GRID,

	EGT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_LANG_ID
{
	ELI_EN_US  = 0,
	ELI_ZH_CN =4,
	ELI_ZH_TW = 5,

	ELI_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

inline const c8* getLangString(E_LANG_ID langid)
{
	switch (langid)
	{
	case ELI_EN_US:
		return "enUS";
	case ELI_ZH_CN:
		return "zhCN";
	case ELI_ZH_TW:
		return "zhTW";
	default:
		_ASSERT(false);
	}
	return NULL;
}

enum E_VS_TYPE
{
	EVST_HWSKINNING = 0,

	EVST_COUNT,

	EVST_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_PS_TYPE
{
	EPST_GENERIC = 0,

	EPST_ALPHATEST,

	EPST_COUNT,

	EPST_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_EFFECT_TYPE
{
	EET_BASICHLSL = 0,
	EET_HWSKINNING,

	EET_COUNT,

	EET_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum E_RENDERINST_TYPE
{
	ERT_NONE = 0,
	ERT_STATIC_MESH = 1,		//地形mesh
	ERT_TERRAIN = 2,			//地形
	ERT_MESH = 4,					//动态模型
	ERT_EFFECT = 8,					//特效
	ERT_WIRE = 16,						//编辑

	ERT_COUNT,

	ERT_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};
