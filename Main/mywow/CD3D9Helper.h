#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "base.h"

#include "SColor.h"
#include <d3d9.h>

class CD3D9Helper
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9Helper);

public:
	static bool init();

public:
	static D3DCOLORVALUE colorToD3D(const SColor& col);

	static D3DCOLORVALUE colorfToD3D(const SColorf& col);

	static D3DTEXTUREADDRESS getD3DTextureAddress( E_TEXTURE_CLAMP clamp);

	static E_TEXTURE_CLAMP getTextureAddressMode(D3DTEXTUREADDRESS address);

	static ECOLOR_FORMAT getColorFormatFromD3DFormat(D3DFORMAT format);

	static D3DFORMAT	getD3DFormatFromColorFormat( ECOLOR_FORMAT format );

	static const char* getD3DFormatString(D3DFORMAT format);

	static uint32_t getD3DBlend( E_BLEND_FACTOR factor );

	static uint32_t getD3DBlendOp( E_BLEND_OP op );

	static uint32_t getD3DTextureOp( E_TEXTURE_OP op );

	static uint32_t getD3DTextureArg( E_TEXTURE_ARG arg );

	static uint32_t getD3DCompare( E_COMPARISON_FUNC comp );

	static D3DFORMAT getD3DIndexType(E_INDEX_TYPE type);

	static D3DPRIMITIVETYPE getD3DTopology(E_PRIMITIVE_TYPE type);
};

inline D3DCOLORVALUE CD3D9Helper::colorToD3D( const SColor& col )
{
	const float f = 1.0f / 255.0f;
	D3DCOLORVALUE v;
	v.r = col.getRed() * f;
	v.g = col.getGreen() * f;
	v.b = col.getBlue() * f;
	v.a = col.getAlpha() * f;
	return v;
}

inline D3DCOLORVALUE CD3D9Helper::colorfToD3D( const SColorf& col )
{
	D3DCOLORVALUE v;
	v.r = col.getRed();
	v.g = col.getGreen();
	v.b = col.getBlue();
	v.a = col.getAlpha();
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


inline E_TEXTURE_CLAMP CD3D9Helper::getTextureAddressMode( D3DTEXTUREADDRESS address )
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
	case D3DFMT_A8:
		return ECF_A8;
	case D3DFMT_A8L8:
		return ECF_A8L8;
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		return ECF_A1R5G5B5;
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return ECF_A8R8G8B8;
	case D3DFMT_R5G6B5:
		return ECF_R5G6B5;
	case D3DFMT_R8G8B8:
		return ECF_R8G8B8;
	case D3DFMT_A32B32G32R32F:
		return ECF_ARGB32F;
	case D3DFMT_D16:
		return ECF_D16;
	case D3DFMT_D24X8:
		return ECF_D24;
	case D3DFMT_D24S8 :
		return ECF_D24S8;

	case D3DFMT_DXT1:
		return ECF_DXT1;
	case D3DFMT_DXT3:
		return ECF_DXT3;
	case D3DFMT_DXT5:
		return ECF_DXT5;
	default:
		ASSERT(false);
		return (ECOLOR_FORMAT)ECF_UNKNOWN;
	};
}

inline D3DFORMAT CD3D9Helper::getD3DFormatFromColorFormat( ECOLOR_FORMAT format )
{
	switch(format)
	{
	case ECF_A8:
		return D3DFMT_A8;
	case ECF_A1R5G5B5:
		return D3DFMT_A1R5G5B5;
	case ECF_R5G6B5:
		return D3DFMT_R5G6B5;
	case ECF_R8G8B8:
		return D3DFMT_R8G8B8;
	case ECF_A8R8G8B8:
		return D3DFMT_A8R8G8B8;
	case ECF_ARGB32F:
		return D3DFMT_A32B32G32R32F;
	case ECF_D16:
		return D3DFMT_D16;
	case ECF_D24:
		return D3DFMT_D24X8;
	case ECF_D24S8:
		return D3DFMT_D24S8;
	case ECF_A8L8:
		return D3DFMT_A8L8;
	case ECF_DXT1:
		return D3DFMT_DXT1;
	case ECF_DXT3:
		return D3DFMT_DXT3;
	case ECF_DXT5:
		return D3DFMT_DXT5;
	}
	ASSERT(false);
	return D3DFMT_UNKNOWN;
}

inline uint32_t CD3D9Helper::getD3DBlend(E_BLEND_FACTOR factor)
{
	uint32_t r = 0;
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
	default:
		ASSERT(false);
	}
	return r;
}

inline uint32_t CD3D9Helper::getD3DBlendOp(E_BLEND_OP op)
{
	uint32_t r = D3DBLENDOP_ADD;
	switch(op)
	{
	case EBO_ADD:
		r = D3DBLENDOP_ADD;
		break;
	case EBO_SUBSTRACT:
		r = D3DBLENDOP_SUBTRACT;
		break;
	default:
		ASSERT(false);
	}
	return r;
}

inline uint32_t CD3D9Helper::getD3DTextureOp( E_TEXTURE_OP op )
{
	uint32_t r = D3DTOP_DISABLE;
	switch(op)
	{
	case ETO_DISABLE:
		r = D3DTOP_DISABLE; break;
	case ETO_ARG1:
		r = D3DTOP_SELECTARG1; break;
	case ETO_MODULATE:
		r = D3DTOP_MODULATE; break;
	case ETO_MODULATE_X2:
		r = D3DTOP_MODULATE2X; break;
	case ETO_MODULATE_X4:
		r = D3DTOP_MODULATE4X; break;
	default:
		ASSERT(false);
	}
	return r;
}

inline uint32_t CD3D9Helper::getD3DTextureArg( E_TEXTURE_ARG arg )
{
	uint32_t r = D3DTA_CURRENT;
	switch(arg)
	{
	case ETA_CURRENT:
		r = D3DTA_CURRENT; break;
	case ETA_TEXTURE:
		r = D3DTA_TEXTURE; break;
	case ETA_DIFFUSE:
		r = D3DTA_DIFFUSE; break;
	default:
		ASSERT(false);
	}
	return r;
}

inline uint32_t CD3D9Helper::getD3DCompare( E_COMPARISON_FUNC comp )
{
	uint32_t r = D3DCMP_NEVER;
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
		ASSERT(false);
	}
	return r;
}

inline D3DFORMAT CD3D9Helper::getD3DIndexType( E_INDEX_TYPE type )
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

inline D3DPRIMITIVETYPE CD3D9Helper::getD3DTopology( E_PRIMITIVE_TYPE type )
{
	switch (type)
	{
	case EPT_POINTS:
		return D3DPT_POINTLIST;
	case EPT_LINE_STRIP:
		return D3DPT_LINESTRIP;
	case EPT_LINES:
		return D3DPT_LINELIST;
	case EPT_TRIANGLE_STRIP:
		return D3DPT_TRIANGLESTRIP;
	case EPT_TRIANGLES:
		return D3DPT_TRIANGLELIST;
	default:
		ASSERT(false);
		return D3DPT_TRIANGLELIST;
	}
}

#endif