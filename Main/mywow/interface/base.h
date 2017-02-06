#pragma once

#include "compileconfig.h"
#include <cassert>
#include <cstdint>
#include "qzone_allocator.h"

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&);               \
TypeName& operator=(const TypeName&)

#define DECLARE_INSTANCE(type)	\
public: static type& getInstance()	 { static type inst; return inst; }

#define ROUND_N_BYTES(x, n) ((x+(n-1)) & ~(n-1))
#define ROUND_4BYTES(x) ((x+3) & ~3)
#define ROUND_8BYTES(x) ((x+7) & ~7)
#define ROUND_16BYTES(x) ((x+15) & ~15)
#define ROUND_32BYTES(x) ((x+31) & ~31)

typedef		uint8_t				u8;
typedef		int8_t				s8;

typedef		char				c8;
typedef		char16_t			c16;

typedef		uint16_t			u16;
typedef		int16_t				s16;

typedef		uint32_t			u32;
typedef		int32_t				s32;

typedef		uint64_t			u64;
typedef		int64_t				s64;

typedef		float				f32;

typedef		char				UTF8;
typedef		char16_t			UTF16;
typedef		char32_t			UTF32;

typedef		uintptr_t		ptr_t;
typedef		intptr_t		sptr_t;

#define PTR_TO_INT32(x)		((s32)((sptr_t)(x) & 0xffffffff))
#define PTR_TO_UINT32(x)	((u32)((ptr_t)(x) & 0xffffffff))

#ifdef MW_PLATFORM_WINDOWS

typedef		HWND				window_type;
typedef		HDC					dc_type;
typedef		HGLRC				glcontext_type;

#else

typedef		void*				HANDLE;
typedef		void*				window_type;
typedef		void*				dc_type;
typedef		void*				glcontext_type;

//	typedef	double	f64;
#endif

//arm下的float, double和u64,s64在赋值时需要4字节对齐，必须注意struct内的对齐问题
#define MAKE_ALIGN4BYTES(x) x = (x+3) & ~3;

#define MAKE_ALIGN4BYTES_POINTER(p, base)	{ u32 len = p - base;	\
	MAKE_ALIGN4BYTES(len)	\
	p = len + base; }	

#ifndef QMAX_PATH
#define		QMAX_PATH	512
#endif

#ifndef ASSERT
#define ASSERT assert
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef NULL_PTR
#define  NULL_PTR	nullptr
#endif

#ifndef MAX
    #define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif

#ifndef DELETE_ARRAY
#define DELETE_ARRAY(t, p)		{ delete[] (static_cast<t*>(p)); }   
#endif

#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( \
	(char*)(address) - \
	(ptr_t)(&((type *)0)->field)))
#endif

#define M_MAKEWORD(a, b)      ((u16)(((u8)(((ptr_t)(a)) & 0xff)) | ((u16)((u8)(((ptr_t)(b)) & 0xff))) << 8))

#define F32_AS_DWORD(f)		(*((u32*)&(f)))
#define DWORD_AS_F32(d)		(*((f32*)&(d)))

#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))

#define		NAME_SIZE   32
#define		DEFAULT_SIZE	64
#define		FONT_TEXTURE_SIZE	512

#define A_MIN_BYTE		0x00
#define A_MAX_BYTE		0xff
#define A_MIN_CHAR		0x80
#define A_MAX_CHAR		0x7f
#define A_MIN_UINT16	0x0000U
#define A_MAX_UINT16	0xffffU
#define A_MIN_UINT32	0x00000000U
#define A_MAX_UINT32	0xffffffffU

#if defined (MW_COMPILE_WITH_GLES2) || (defined (MW_COMPILE_WITH_OPENGL) && defined(USE_WITH_GLES2))
#define		MAX_BONE_NUM		35
#define		MAX_TEXT_LENGTH		256			//每批次最大渲染字数
#else
#define		MAX_BONE_NUM		58
#define		MAX_TEXT_LENGTH		256			//每批次最大渲染字数
#endif

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

//terrain
#define TILESIZE (533.33333f)
#define CHUNKSIZE ((TILESIZE) / 16.0f)
#define UNITSIZE (CHUNKSIZE / 8.0f)
#define ZEROPOINT (32.0f * (TILESIZE))

#define	CHUNKS_IN_TILE	16				//每个tile包括 16 X 16 个chunk

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL_PTR; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)		{ if (p) { delete (p); (p) = NULL_PTR; } }
#endif

#ifndef SAFE_RELEASE_STRICT
#define SAFE_RELEASE_STRICT(p)      { if (p) { ULONG u = (p)->Release(); ASSERT(!u); (p)=NULL_PTR; } }
#endif

#ifndef RELEASE_ALL
#define RELEASE_ALL(x)			\
	ULONG rest = x->Release();	\
	while( rest > 0 )			\
	rest = x->Release();	\
	x = 0;
#endif

#ifndef ARRAY_COUNT
#define ARRAY_COUNT(a)		(sizeof(a)/sizeof(a[0]))
#endif

enum E_LOG_TYPE : int32_t
{
	ELOG_GX = 0,
	ELOG_RES,
	ELOG_SOUND,
	ELOG_UI,
};

enum E_DRIVER_TYPE : int32_t
{
	EDT_NULL = 0,
	EDT_DIRECT3D9,
	EDT_DIRECT3D11,
	EDT_OPENGL,
	EDT_COUNT,
};

inline const c8* getEnumString(E_DRIVER_TYPE type)
{
	switch(type)
	{
	case EDT_DIRECT3D9:
		return "Direct3D9";
	case EDT_DIRECT3D11:
		return "Direct3D11";
	case EDT_OPENGL:
		return "OpenGL";
	default:
		return "Unknown";
	}
}

enum E_TRANSFORMATION_STATE : int32_t
{
	ETS_VIEW = 0,
	ETS_WORLD,
	ETS_PROJECTION,
	ETS_TEXTURE_0,
	ETS_TEXTURE_1,
	ETS_TEXTURE_2,
	ETS_TEXTURE_3,
	ETS_COUNT,
};

enum E_VIDEO_DRIVER_FEATURE : int32_t
{
	EVDF_RENDER_TO_TARGET = 0,
	EVDF_HARDWARE_TL,
	EVDF_TEXTURE_ADDRESS,
	EVDF_SEPARATE_UVWRAP,
	EVDF_MIP_MAP,
	EVDF_STENCIL_BUFFER,
	EVDF_VERTEX_SHADER_2_0,
	EVDF_VERTEX_SHADER_3_0,
	EVDF_PIXEL_SHADER_2_0,
	EVDF_PIXEL_SHADER_3_0,
	EVDF_TEXTURE_NSQUARE,
	EVDF_TEXTURE_NPOT,
	EVDF_COLOR_MASK,
	EVDF_MULTIPLE_RENDER_TARGETS,
	EVDF_MRT_COLOR_MASK,
	EVDF_MRT_BLEND_FUNC,
	EVDF_MRT_BLEND,
	EVDF_STREAM_OFFSET,
	EVDF_W_BUFFER,

	//! Supports Shader model 4
	EVDF_VERTEX_SHADER_4_0,
	EVDF_PIXEL_SHADER_4_0,
	EVDF_GEOMETRY_SHADER_4_0,
	EVDF_STREAM_OUTPUT_4_0,
	EVDF_COMPUTING_SHADER_4_0,

	//! Supports Shader model 4.1
	EVDF_VERTEX_SHADER_4_1,
	EVDF_PIXEL_SHADER_4_1,
	EVDF_GEOMETRY_SHADER_4_1,
	EVDF_STREAM_OUTPUT_4_1,
	EVDF_COMPUTING_SHADER_4_1,

	//! Supports Shader model 5.0
	EVDF_VERTEX_SHADER_5_0,
	EVDF_PIXEL_SHADER_5_0,
	EVDF_GEOMETRY_SHADER_5_0,
	EVDF_STREAM_OUTPUT_5_0,
	EVDF_TESSELATION_SHADER_5_0,
	EVDF_COMPUTING_SHADER_5_0,

	//! Supports texture multisampling
	EVDF_TEXTURE_MULTISAMPLING,
	EVDF_COUNT,
};

enum ECOLOR_FORMAT : int32_t
{
	ECF_UNKNOWN = 0,
	
	//8
	ECF_A8,
	//16
	ECF_A8L8,

	ECF_A1R5G5B5,
	ECF_R5G6B5,

	//24
	ECF_R8G8B8,

	//32
	ECF_A8R8G8B8,

	//float for RenderTarget Buffer 
	ECF_ARGB32F,

	//DXT
	ECF_DXT1,
	ECF_DXT3,
	ECF_DXT5,

	//PVR
	ECF_PVRTC1_RGB_2BPP,
	ECF_PVRTC1_RGBA_2BPP,
	ECF_PVRTC1_RGB_4BPP,
	ECF_PVRTC1_RGBA_4BPP,

	//ETC
	ECF_ETC1_RGB,
	ECF_ETC1_RGBA,

	//ATC
	ECF_ATC_RGB,
	ECF_ATC_RGBA_EXPLICIT,
	ECF_ATC_RGBA_INTERPOLATED,

	//DEPTH
	ECF_D16,
	ECF_D24,
	ECF_D24S8,
	ECF_D32,
};

struct STexFormatDesc 
{
	ECOLOR_FORMAT format;
	u32 blockBytes;
	u32 blockWidth;
	u32 blockHeight;
	u32 minWidth;
	u32 minHeight;
	c8 text[32];
};

static STexFormatDesc g_FormatDesc[] =
{
	{ ECF_UNKNOWN,					0, 0, 0, 0, 0,		"UNKNOWN", },
	{ ECF_A8,									1, 1, 1, 1, 1,		"A8", },
	{ ECF_A8L8,								2, 1, 1, 1, 1,		"A8L8", },
	{ ECF_A1R5G5B5,						2, 1, 1, 1, 1,		"A1R5G5B5", },
	{ ECF_R5G6B5,							2, 1, 1, 1, 1,		"R5G6B5", },
	{ ECF_R8G8B8,							3, 1, 1, 1, 1,		"R8G8B8", },
	{ ECF_A8R8G8B8,						4, 1, 1, 1, 1,		"A8R8G8B8", },
	{ ECF_ARGB32F,						16, 1, 1, 1, 1,		"ARGB32F", },
	{ ECF_DXT1,								8, 4, 4, 4, 4,		"DXT1", },
	{ ECF_DXT3,								16, 4, 4, 4, 4,		"DXT3", },
	{ ECF_DXT5,								16, 4, 4, 4, 4,		"DXT5", },
	{ ECF_PVRTC1_RGB_2BPP,		8, 8, 4, 16, 8,		"PVRTC1_RGB_2BPP", },
	{ ECF_PVRTC1_RGBA_2BPP,	8, 8, 4, 16, 8,		"PVRTC1_RGBA_2BPP", },
	{ ECF_PVRTC1_RGB_4BPP,		8, 4, 4, 8, 8,		"PVRTC1_RGB_4BPP", },
	{ ECF_PVRTC1_RGBA_4BPP,	8, 4, 4, 8, 8,		"PVRTC1_RGBA_4BPP", },
	{ ECF_ETC1_RGB,						8, 4, 4, 4, 4,		"ETC1_RGB", },
	{ ECF_ETC1_RGBA,					8, 4, 4, 4, 4,		"ETC1_RGBA", },
	{ ECF_ATC_RGB,						8, 4, 4, 4, 4,		"ATC_RGB", },
	{ ECF_ATC_RGBA_EXPLICIT,	16, 4, 4, 4, 4,		"ATC_RGBA_EXPLICIT", },
	{ ECF_ATC_RGBA_INTERPOLATED, 16, 4, 4, 4, 4,	"ATC_RGBA_INTERPOLATED", },
	{ ECF_D16, 2, 1, 1, 1, 1,	"DEPTH16", },
	{ ECF_D24, 4, 1, 1, 1, 1,	"DEPTH24", },
	{ ECF_D24S8, 4, 1, 1, 1, 1,	"DEPTH24STENCIL8", },
	{ ECF_D32, 4, 1, 1, 1, 1,	"DEPTH32", },
};

inline u32 getBytesPerPixelFromFormat( ECOLOR_FORMAT format)
{
	ASSERT(static_cast<u32>(format) < ARRAY_COUNT(g_FormatDesc));
	return g_FormatDesc[format].blockBytes;
}

inline bool isCompressedFormat( ECOLOR_FORMAT format )
{
	ASSERT(static_cast<u32>(format) < ARRAY_COUNT(g_FormatDesc));
	return g_FormatDesc[format].blockWidth > 1;
}

inline bool isCompressedWithAlphaFormat(ECOLOR_FORMAT format)
{
	return format == ECF_ETC1_RGBA;
}

inline const c8* getColorFormatString( ECOLOR_FORMAT format )
{
	ASSERT(static_cast<u32>(format) < ARRAY_COUNT(g_FormatDesc));
	return g_FormatDesc[format].text;
}

inline void getImageSize( ECOLOR_FORMAT format, u32 width, u32 height, u32& w, u32& h)
{
	ASSERT(static_cast<u32>(format) < ARRAY_COUNT(g_FormatDesc));

	u32 bw = g_FormatDesc[format].blockWidth;
	u32 bh = g_FormatDesc[format].blockHeight;
    
    u32 mw = g_FormatDesc[format].minWidth;
    u32 mh = g_FormatDesc[format].minHeight;
    
	if (bw > 1)			//compressed
	{
		w = MAX(mw, (width + (bw - 1))) / bw;
		h = MAX(mh, (height + (bh - 1))) / bh;
	}
	else
	{
		w = width;
		h = height;
	}
}

inline void getImagePitchAndBytes( ECOLOR_FORMAT format, u32 width, u32 height, u32& pitch, u32& bytes)
{
	u32 bpp = getBytesPerPixelFromFormat(format);

	u32 w, h;
	getImageSize(format, width, height, w, h);

	pitch = w * bpp;
	bytes = pitch * h;
}

enum E_SHADER_VERSION : int32_t
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
};

enum E_STREAM_TYPE : int32_t
{
	EST_INVALID = -1,
	EST_P = 0,
	EST_PC,
	EST_PCT,
	EST_PN,
	EST_PNC,
	EST_PNT,
	EST_PT,
	EST_PNCT,
	EST_PNCT2,
	EST_PNT2W,
	EST_A,
};

enum E_VERTEX_TYPE : int32_t
{
	EVT_INVALID = -1,
	EVT_P = 0,
	EVT_PC,			//for bounding box
	EVT_PCT,
	EVT_PN,
	EVT_PNC,
	EVT_PNT,
	EVT_PT,
	EVT_PNCT,
	EVT_PNCT2,
	EVT_PNT2W,						//fvf
	EVT_COUNT,
};

inline E_VERTEX_TYPE getVertexType(E_STREAM_TYPE streamtype)
{
	switch(streamtype)
	{
	case EST_P: return EVT_P;
	case EST_PC: return EVT_PC;
	case EST_PCT: return EVT_PCT;
	case EST_PN: return EVT_PN;
	case EST_PNC: return EVT_PNC;
	case EST_PNT: return EVT_PNT;
	case EST_PT: return EVT_PT;
	case EST_PNCT: return EVT_PNCT;
	case EST_PNCT2: return EVT_PNCT2;
	case EST_PNT2W: return EVT_PNT2W;
	default:
		return EVT_INVALID;
	}
}

inline bool hasTexture(E_VERTEX_TYPE vType)
{
	switch(vType)
	{
	case EVT_PCT:
	case EVT_PNT:
	case EVT_PT:
	case EVT_PNCT:
	case EVT_PNCT2:
	case EVT_PNT2W:
		return true;
	default:
		return false;
	}
}

enum E_MESHBUFFER_MAPPING : int32_t
{
	EMM_SOFTWARE = 0,					
	EMM_STATIC,
	EMM_DYNAMIC,
	EMM_COUNT,
};

enum E_INDEX_TYPE : int32_t
{
	EIT_16BIT = 0,
	EIT_32BIT,
	EIT_COUNT,
};

enum E_PRIMITIVE_TYPE : int32_t
{
	EPT_POINTS=0,
	EPT_LINE_STRIP,
	EPT_LINES,
	EPT_TRIANGLE_STRIP,
	EPT_TRIANGLES,
	EPT_COUNT,
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
		ASSERT(count>=2);
		p = count/2;
		break;
	case EPT_LINE_STRIP:
		ASSERT(count>=2);
		p = count - 1;
		break;
	case EPT_TRIANGLE_STRIP:
		ASSERT(count>=3);
		p = count - 2;
		break;
	case EPT_TRIANGLES:
		ASSERT(count>=3);
		p = count / 3;
		break;
	default:
		ASSERT(false);
	}
	return p;
}

inline u32 getIndexCount(E_PRIMITIVE_TYPE primType, u32 primCount)
{
	u32 p = 0;

	switch (primType)
	{
	case EPT_POINTS:
		p= primCount;
		break;
	case EPT_LINES:
		p = primCount * 2;
		break;
	case EPT_LINE_STRIP:
		p = primCount + 1;
		break;
	case EPT_TRIANGLE_STRIP:
		p = primCount + 2;
		break;
	case EPT_TRIANGLES:
		p = primCount * 3;
		break;
	default:
		ASSERT(false);
	}
	return p;
}

enum E_MATERIAL_TYPE
{
	EMT_2D = 0,
	EMT_LINE,
	EMT_SOLID,

	EMT_TERRAIN_MULTIPASS,

	EMT_ALPHA_TEST,	
	EMT_TRANSAPRENT_ALPHA_BLEND_TEST,
	EMT_TRANSPARENT_ALPHA_BLEND,
	EMT_TRANSPARENT_ONE_ALPHA,
	EMT_TRANSPARENT_ADD_ALPHA,	
	EMT_TRANSPARENT_ADD_COLOR,
	EMT_TRANSPARENT_MODULATE,
	EMT_TRANSPARENT_MODULATE_X2,
	
	EMT_COUNT,

	EMT_FORCE_8BIT_DO_NOT_USE = 0x7f
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

	ECFN_FORCE_8_BIT_DO_NOT_USE = 0x7f
};

enum E_BLEND_OP
{
	EBO_ADD = 0,
	EBO_SUBSTRACT,

	EBO_FORCE_8_BIT_DO_NOT_USE = 0x7f
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

	EBF_FORCE_8_BIT_DO_NOT_USE = 0x7f
};

enum E_TEXTURE_OP
{
	ETO_DISABLE = 0,
	ETO_ARG1,
	ETO_MODULATE,
	ETO_MODULATE_X2,
	ETO_MODULATE_X4,

	ETO_FORCE_8_BIT_DO_NOT_USE = 0x7f
};

enum E_TEXTURE_ARG
{
	ETA_CURRENT = 0,
	ETA_TEXTURE,
	ETA_DIFFUSE,

	ETA_FORCE_8_BIT_DO_NOT_USE = 0x7f
};

enum E_TEXTURE_ADDRESS
{
	ETA_U = 0,
	ETA_V,
	ETA_W,
	ETA_COUNT,
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

	ETC_FORCE_8_BIT_DO_NOT_USE = 0x7f
};

enum E_TEXTURE_FILTER : int32_t
{
	ETF_NONE = 0,
	ETF_BILINEAR,
	ETF_TRILINEAR,
	ETF_ANISOTROPIC_X1,
	ETF_ANISOTROPIC_X2,
	ETF_ANISOTROPIC_X4,
	ETF_ANISOTROPIC_X8,
	ETF_ANISOTROPIC_X16,
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
	case ETF_ANISOTROPIC_X16:
		return 16;
	default:
		return 1;
	}
}


#define  MATERIAL_MAX_TEXTURES		6

enum E_LIGHT_TYPE : int32_t
{
	ELT_POINT = 0,
	ELT_SPOT,
	ELT_DIRECTIONAL,
	ELT_AMBIENT,
};

enum E_CULL_MODE
{
	ECM_NONE = 0,
	ECM_FRONT,
	ECM_BACK,

	ECM_FORCE_8_BIT_DO_NOT_USE = 0x7f
};

enum E_ANTI_ALIASING_MODE
{
	EAAM_OFF = 0,
	EAAM_SIMPLE,
	EAAM_LINE_SMOOTH,

	EAAM_FORCE_8_BIT_DO_NOT_USE = 0x7f
};

//雾
enum E_FOG_TYPE : int32_t
{
	EFT_FOG_NONE=0,
	EFT_FOG_EXP,
	EFT_FOG_EXP2,
	EFT_FOG_LINEAR,
};

enum E_GEOMETRY_TYPE : int32_t
{
	EGT_CUBE = 0,
	EGT_SPHERE,
	EGT_SKYDOME,
	EGT_GRID,
};

enum E_VS_TYPE : int32_t
{
	EVST_TERRAIN = 0,
	EVST_CDIFFUSE_T1,
	EVST_DIFFUSE_T1,
	EVST_DIFFUSE_ENV,
	EVST_DIFFUSE_T1_T1,
	EVST_DIFFUSE_T1_T2,
	EVST_DIFFUSE_T1_ENV,
	EVST_DIFFUSE_ENV_T1,
	EVST_DIFFUSE_ENV_ENV,
	EVST_DIFFUSE_T1_T1_T1,
	EVST_DIFFUSE_T1_T2_T1,
	EVST_DIFFUSE_T1_ENV_T1,
	EVST_DIFFUSE_T1_ENV_T2,

	EVST_DEFAULT_P,
	EVST_DEFAULT_PC,
	EVST_DEFAULT_PCT,
	EVST_DEFAULT_PN,
	EVST_DEFAULT_PNC,
	EVST_DEFAULT_PNCT,
	EVST_DEFAULT_PNCT2,
	EVST_DEFAULT_PNT,
	EVST_DEFAULT_PT,
	
	EVST_MAPOBJ_DIFFUSE_T1,
	EVST_MAPOBJ_SPECULAR_T1,
	EVST_MAPOBJ_DIFFUSE_T1_T2,
	EVST_MAPOBJ_DIFFUSE_T1_ENV_T2,
	EVST_MAPOBJ_DIFFUSE_T1_REFL,

	EVST_COUNT,
};

enum E_PS_TYPE : int32_t
{

	EPST_DEFAULT_P = 0,
	EPST_DEFAULT_PC,
	EPST_DEFAULT_PCT,
	EPST_DEFAULT_PN,
	EPST_DEFAULT_PNC,
	EPST_DEFAULT_PNCT,
	EPST_DEFAULT_PNCT2,
	EPST_DEFAULT_PNT,
	EPST_DEFAULT_PT,

	EPST_UI,
	EPST_UI_ALPHA,
	EPST_UI_ALPHACHANNEL,
	EPST_UI_ALPHA_ALPHACHANNEL,
	EPST_TERRAIN_1LAYER,
	EPST_TERRAIN_2LAYER,
	EPST_TERRAIN_3LAYER,
	EPST_TERRAIN_4LAYER,

	//blend
	EPST_COMBINERS_MOD,
	EPST_COMBINERS_MOD_ADD,
	EPST_COMBINERS_MOD_ADD_ALPHA,
	EPST_COMBINERS_MOD_ADDALPHA,
	EPST_COMBINERS_MOD_ADDALPHA_ALPHA,
	EPST_COMBINERS_MOD_ADDALPHA_WGT,
	EPST_COMBINERS_MOD_ADDNA,
	EPST_COMBINERS_MOD_MOD,
	EPST_COMBINERS_MOD_MOD2X,
	EPST_COMBINERS_MOD_MOD2XNA,
	EPST_COMBINERS_MOD_OPAQUE,
	EPST_COMBINERS_OPAQUE,
	EPST_COMBINERS_OPAQUE_ADDALPHA,
	EPST_COMBINERS_OPAQUE_ADDALPHA_ALPHA,
	EPST_COMBINERS_OPAQUE_ADDALPHA_WGT,
	EPST_COMBINERS_OPAQUE_ALPHA,
	EPST_COMBINERS_OPAQUE_ALPHA_ALPHA,
	EPST_COMBINERS_OPAQUE_MOD,
	EPST_COMBINERS_OPAQUE_MOD2X,
	EPST_COMBINERS_OPAQUE_MOD2XNA,
	EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA,
	EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_3S,
	EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ADD,
	EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ALPHA,
	EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_UNSHALPHA,
	EPST_COMBINERS_OPAQUE_MOD_ADD_WGT,
	EPST_COMBINERS_OPAQUE_MODNA_ALPHA,
	EPST_COMBINERS_OPAQUE_OPAQUE,
	
	EPST_MAPOBJ_DIFFUSE,
	EPST_MAPOBJ_DIFFUSEEMISSIVE,
	EPST_MAPOBJ_OPAQUE,
	EPST_MAPOBJ_SPECULAR,
	EPST_MAPOBJ_METAL,
	EPST_MAPOBJ_TWOLAYERDIFFUSE,
	EPST_MAPOBJ_ENVMETAL,
	EPST_MAPOBJ_ENV,
	EPST_MAPOBJ_TWOLAYERDIFFUSEOPAQUE,
	EPST_MAPOBJ_TWOLAYERENVMETAL,
	EPST_MAPOBJ_TWOLAYERTERRAIN,

	EPST_COUNT,
};

enum E_UNIFORM_TYPE : int32_t
{
	EUT_FLOAT = 0,
	EUT_VEC2,
	EUT_VEC3,
	EUT_VEC4,
	EUT_MAT2,
	EUT_MAT3,
	EUT_MAT4,
	EUT_SAMPLER1D,
	EUT_SAMPLER2D,
	EUT_SAMPLER3D,

	EUT_COUNT,
};

enum E_MATERIAL_LIGHT : int32_t
{
	EML_AMBIENT = 0,
	EML_DIFFUSE,
	EML_SPECULAR,

	EML_COUNT
};

enum E_RENDERINST_TYPE : int32_t
{
	ERT_NONE = 0,
	ERT_SKY,
	ERT_TERRAIN,			//低精度地形
	ERT_WMO,					//建筑
	ERT_DOODAD,		//地形小物件
	ERT_DOODADDECAL,		//m2中的decal
	ERT_MESH,					//角色模型,m2
	ERT_MESHDECAL,		//m2中的decal
	ERT_ALPHATEST,
	ERT_TRANSPARENT,
	ERT_PARTICLE,					//特效
	ERT_RIBBON,

	ERT_WIRE,						//编辑

	ERT_COUNT,
};

enum E_LEVEL : int32_t
{
	EL_DISABLE = 0,
	EL_LOW,
	EL_FAIR,
	EL_GOOD,
	EL_HIGH,
	EL_ULTRA,
};

enum E_ADT_LOAD : int32_t
{
	EAL_3X3 = 0,
	EAL_5X5,
//	EAL_7X7,		//7X7 is not used yet :(
};

class IAdtLoadSizeChangedCallback
{
public:
	virtual void OnAdtLoadSizeChanged(E_ADT_LOAD size) = 0;
};

enum E_INPUT_DEVICE : int32_t
{
	EID_KEYBOARD = 0,
	EID_MOUSE,
	EID_JOYSTICK,
};

enum E_MOUSE_BUTTON : int32_t
{
	EMB_NONE = 0,
	EMB_LEFT = 1,
	EMB_RIGHT = 2,
	EMB_MIDDLE = 4,
};
 
enum E_MODIFIER_KEY : int32_t
{
	EMK_NONE = 0,
	EMK_SHIFT = 1,
	EMK_CTRL = 2,
	EMK_ALT = 4,
	EMK_LEFTMOUSE = 8,
	EMK_RIGHTMOUSE = 16,
	EMK_MIDDLEMOUSE = 32,
};

enum E_INPUT_MESSAGE : int32_t
{
	InputMessage_None = 0,
	Mouse_LeftButtonDown = 1,
	Mouse_LeftButtonUp,
	Mouse_RightButtonDown,
	Mouse_RightButtonUp,
	Mouse_Move,

	Key_Down = 100,
	Key_Up,
};

enum E_RECT_UVCOORDS : int32_t
{
	ERU_00_11 = 0,
	ERU_01_10,
	ERU_10_01,
	ERU_11_00,
};

enum E_FONT_TYPE : int32_t
{
	EFT_DEFAULT = 0,

	EFT_NUM,
};
