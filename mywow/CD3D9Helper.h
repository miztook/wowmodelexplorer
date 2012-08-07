#pragma once

#include "base.h"

#include "SColor.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <DxErr.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

//imports

typedef HRESULT (WINAPI *D3DXASSEMBLESHADERFROMFILEA)(
	LPCSTR                          pSrcFile,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	DWORD                           Flags,
	LPD3DXBUFFER*                   ppShader,
	LPD3DXBUFFER*                   ppErrorMsgs);

typedef HRESULT (WINAPI *D3DXASSEMBLESHADERFROMFILEW)(
	LPCWSTR                         pSrcFile,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	DWORD                           Flags,
	LPD3DXBUFFER*                   ppShader,
	LPD3DXBUFFER*                   ppErrorMsgs);

typedef HRESULT (WINAPI *D3DXCOMPILESHADERFROMFILEA)(
	LPCSTR                          pSrcFile,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	LPCSTR                          pFunctionName,
	LPCSTR                          pProfile,
	DWORD                           Flags,
	LPD3DXBUFFER*                   ppShader,
	LPD3DXBUFFER*                   ppErrorMsgs,
	LPD3DXCONSTANTTABLE*            ppConstantTable);

typedef HRESULT (WINAPI *D3DXCOMPILESHADERFROMFILEW)(
	LPCWSTR                         pSrcFile,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	LPCSTR                          pFunctionName,
	LPCSTR                          pProfile,
	DWORD                           Flags,
	LPD3DXBUFFER*                   ppShader,
	LPD3DXBUFFER*                   ppErrorMsgs,
	LPD3DXCONSTANTTABLE*            ppConstantTable);

typedef HRESULT (WINAPI *D3DXCREATEEFFECTFROMFILEA)(
	LPDIRECT3DDEVICE9               pDevice,
	LPCSTR                          pSrcFile,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	DWORD                           Flags,
	LPD3DXEFFECTPOOL                pPool,
	LPD3DXEFFECT*                   ppEffect,
	LPD3DXBUFFER*                   ppCompilationErrors);

typedef HRESULT (WINAPI *D3DXCREATEEFFECTFROMFILEW)(
	LPDIRECT3DDEVICE9               pDevice,
	LPCWSTR                         pSrcFile,
	CONST D3DXMACRO*                pDefines,
	LPD3DXINCLUDE                   pInclude,
	DWORD                           Flags,
	LPD3DXEFFECTPOOL                pPool,
	LPD3DXEFFECT*                   ppEffect,
	LPD3DXBUFFER*                   ppCompilationErrors);

typedef HRESULT (WINAPI *D3DXCREATEBUFFER)(
	DWORD NumBytes, 
	LPD3DXBUFFER *ppBuffer);

typedef HRESULT (WINAPI *D3DXDISASSEMBLESHADER)(
	CONST DWORD*                    pShader, 
	BOOL                            EnableColorCode, 
	LPCSTR                          pComments, 
	LPD3DXBUFFER*                   ppDisassembly);

class CD3D9Helper
{
public:
	CD3D9Helper();

public:
	static D3DCOLORVALUE colorToD3D(const SColor& col);

	static D3DTEXTUREADDRESS getD3DTextureAddress( E_TEXTURE_CLAMP clamp);

	static E_TEXTURE_CLAMP getTextureWrapMode(D3DTEXTUREADDRESS address);

	static ECOLOR_FORMAT getColorFormatFromD3DFormat(D3DFORMAT format);

	static D3DFORMAT	getD3DFormatFromColorFormat( ECOLOR_FORMAT format );

	static u32 getD3DBlend( E_BLEND_FACTOR factor );

	static u32 getD3DTextureOp( E_TEXTURE_OP op );

	static u32 getD3DTextureArg( E_TEXTURE_ARG arg );

	static u32 getD3DCompare( E_COMPARISON_FUNC comp );

	static D3DFORMAT getIndexType(E_INDEX_TYPE type);

	static D3DMATRIX	UnitMatrixD3D9;
	static D3DMATRIX	View2DMatrixD3D9;
	static D3DMATRIX	SphereMapMatrixD3D9;

public:

	static D3DXASSEMBLESHADERFROMFILEW	d3dxAssembleShaderFromFileW;
	static D3DXCOMPILESHADERFROMFILEW d3dxCompileShaderFromFileW;
	static D3DXCREATEEFFECTFROMFILEW d3dxCreateEffectFromFileW;

	static D3DXASSEMBLESHADERFROMFILEA	d3dxAssembleShaderFromFileA;
	static D3DXCOMPILESHADERFROMFILEA d3dxCompileShaderFromFileA;
	static D3DXCREATEEFFECTFROMFILEA d3dxCreateEffectFromFileA;

	static D3DXCREATEBUFFER	d3dxCreateBuffer;
	static D3DXDISASSEMBLESHADER  d3dxDisassembleShader;
};

inline D3DCOLORVALUE CD3D9Helper::colorToD3D( const SColor& col )
{
	const f32 f = 1.0f / 255.0f;
	D3DCOLORVALUE v;
	v.r = col.getRed() * f;
	v.g = col.getGreen() * f;
	v.b = col.getBlue() * f;
	v.a = col.getAlpha() * f;
	return v;
}

inline D3DTEXTUREADDRESS CD3D9Helper::getD3DTextureAddress( E_TEXTURE_CLAMP clamp )
{
	switch (clamp)
	{
	case ETC_REPEAT:
		return D3DTADDRESS_WRAP;
	case ETC_CLAMP:
		return D3DTADDRESS_CLAMP;
	case ETC_MIRROR:
		return D3DTADDRESS_MIRROR;
	case ETC_CLAMP_TO_BORDER:
		return D3DTADDRESS_BORDER;
	case ETC_MIRROR_CLAMP:
	case ETC_MIRROR_CLAMP_TO_BORDER:
		return D3DTADDRESS_MIRRORONCE;
	default:
		return D3DTADDRESS_WRAP;
	}
}


inline E_TEXTURE_CLAMP CD3D9Helper::getTextureWrapMode( D3DTEXTUREADDRESS address )
{
	switch(address)
	{
	case D3DTADDRESS_WRAP:
		return ETC_REPEAT;
	case D3DTADDRESS_CLAMP:
		return ETC_CLAMP;
	case D3DTADDRESS_MIRROR:
		return ETC_MIRROR;
	case D3DTADDRESS_BORDER:
		return ETC_CLAMP_TO_BORDER;
	case D3DTADDRESS_MIRRORONCE:
		return ETC_MIRROR_CLAMP;
	default:
		return ETC_REPEAT;
	}
}

inline ECOLOR_FORMAT CD3D9Helper::getColorFormatFromD3DFormat( D3DFORMAT format )
{
	switch(format)
	{
	case D3DFMT_A8L8:
		return ECF_A8L8;
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		return ECF_A1R5G5B5;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return ECF_A8R8G8B8;
	case D3DFMT_R5G6B5:
		return ECF_R5G6B5;
	case D3DFMT_R8G8B8:
		return ECF_R8G8B8;
	case D3DFMT_DXT1:
		return ECF_DXT1;
	case D3DFMT_DXT2:
		return ECF_DXT2;
	case D3DFMT_DXT3:
		return ECF_DXT3;
	case D3DFMT_DXT4:
		return ECF_DXT4;
	case D3DFMT_DXT5:
		return ECF_DXT5;
	default:
		return (ECOLOR_FORMAT)ECF_UNKNOWN;
	};
}

inline D3DFORMAT CD3D9Helper::getD3DFormatFromColorFormat( ECOLOR_FORMAT format )
{
	switch(format)
	{
	case ECF_A1R5G5B5:
		return D3DFMT_A1R5G5B5;
	case ECF_R5G6B5:
		return D3DFMT_R5G6B5;
	case ECF_R8G8B8:
		return D3DFMT_R8G8B8;
	case ECF_A8R8G8B8:
		return D3DFMT_A8R8G8B8;
	case ECF_A8L8:
		return D3DFMT_A8L8;
	case ECF_DXT1:
		return D3DFMT_DXT1;
	case ECF_DXT2:
		return D3DFMT_DXT2;
	case ECF_DXT3:
		return D3DFMT_DXT3;
	case ECF_DXT4:
		return D3DFMT_DXT4;
	case ECF_DXT5:
		return D3DFMT_DXT5;
	}
	_ASSERT(false);
	return D3DFMT_UNKNOWN;
}

inline u32 CD3D9Helper::getD3DBlend(E_BLEND_FACTOR factor)
{
	u32 r = 0;
	switch ( factor )
	{
	case EBF_ZERO:	
		r = D3DBLEND_ZERO; break;
	case EBF_ONE:	
		r = D3DBLEND_ONE; break;
	case EBF_DST_COLOR:	
		r = D3DBLEND_DESTCOLOR; break;
	case EBF_ONE_MINUS_DST_COLOR:
		r = D3DBLEND_INVDESTCOLOR; break;
	case EBF_SRC_COLOR:		
		r = D3DBLEND_SRCCOLOR; break;
	case EBF_ONE_MINUS_SRC_COLOR:
		r = D3DBLEND_INVSRCCOLOR; break;
	case EBF_SRC_ALPHA:	
		r = D3DBLEND_SRCALPHA; break;
	case EBF_ONE_MINUS_SRC_ALPHA:
		r = D3DBLEND_INVSRCALPHA; break;
	case EBF_DST_ALPHA:	
		r = D3DBLEND_DESTALPHA; break;
	case EBF_ONE_MINUS_DST_ALPHA:
		r = D3DBLEND_INVDESTALPHA; break;
	case EBF_SRC_ALPHA_SATURATE:
		r = D3DBLEND_SRCALPHASAT; break;
	}
	return r;
}

inline u32 CD3D9Helper::getD3DTextureOp( E_TEXTURE_OP op )
{
	u32 r = D3DTOP_DISABLE;
	switch(op)
	{
	case ETO_DISABLE:
		r = D3DTOP_DISABLE; break;
	case ETO_ARG1:
		r = D3DTOP_SELECTARG1; break;
	case ETO_ARG2:
		r = D3DTOP_SELECTARG2; break;
	case ETO_MODULATE:
		r = D3DTOP_MODULATE; break;
	case ETO_MODULATE_X2:
		r = D3DTOP_MODULATE2X; break;
	case ETO_MODULATE_X4:
		r = D3DTOP_MODULATE4X; break;
	case ETO_ADD:
		r = D3DTOP_ADD; break;
	case ETO_ADDSIGNED:
		r = D3DTOP_ADDSIGNED; break;
	default:
		_ASSERT(false);
	}
	return r;
}

inline u32 CD3D9Helper::getD3DTextureArg( E_TEXTURE_ARG arg )
{
	u32 r = D3DTA_CURRENT;
	switch(arg)
	{
	case ETA_CURRENT:
		r = D3DTA_CURRENT; break;
	case ETA_TEXTURE:
		r = D3DTA_TEXTURE; break;
	case ETA_DIFFUSE:
		r = D3DTA_DIFFUSE; break;
	default:
		_ASSERT(false);
	}
	return r;
}

inline u32 CD3D9Helper::getD3DCompare( E_COMPARISON_FUNC comp )
{
	u32 r = D3DCMP_NEVER;
	switch(comp)
	{
	case ECFN_NEVER:
		r = D3DCMP_NEVER; break;
	case	ECFN_LESSEQUAL:
		r = D3DCMP_LESSEQUAL; break;
	case	ECFN_EQUAL:
		r = D3DCMP_EQUAL; break;
	case 	ECFN_LESS:
		r = D3DCMP_LESS; break;
	case	ECFN_NOTEQUAL:
		r = D3DCMP_NOTEQUAL; break;
	case	ECFN_GREATEREQUAL:
		r = D3DCMP_GREATEREQUAL; break;
	case	ECFN_GREATER:
		r = D3DCMP_GREATER;	break;
	case	ECFN_ALWAYS:
		r = D3DCMP_ALWAYS;	break;
	default:
		_ASSERT(false);
	}
	return r;
}

inline D3DFORMAT CD3D9Helper::getIndexType( E_INDEX_TYPE type )
{
	switch(type)
	{
	case EIT_16BIT:
		return D3DFMT_INDEX16;
	case EIT_32BIT:
		return D3DFMT_INDEX32;
	default:
		return D3DFMT_UNKNOWN;
	}
}

extern CD3D9Helper g_d3dHelper;