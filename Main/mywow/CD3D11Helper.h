#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "base.h"
#include "SColor.h"
#include <d3d11.h>

class CD3D11Helper
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11Helper);

public:
	static bool init();

public:

	static ECOLOR_FORMAT getColorFormatFromDXGIFormat(DXGI_FORMAT format);

	static DXGI_FORMAT getDXGIFormatFromColorFormat(ECOLOR_FORMAT format);

	static DXGI_FORMAT getDXGIFormatFromColorFormat(ECOLOR_FORMAT format, DXGI_FORMAT& depthTextureFmt, DXGI_FORMAT& depthSRFmt);

	static D3D11_COMPARISON_FUNC getD3DCompare( E_COMPARISON_FUNC comp );

	static const c8* getD3DFormatString(DXGI_FORMAT format);

	static D3D11_BLEND getD3DBlend( E_BLEND_FACTOR factor );

	static D3D11_BLEND_OP getD3DBlendOp( E_BLEND_OP op );

	static D3D11_TEXTURE_ADDRESS_MODE getD3DTextureAddress( E_TEXTURE_CLAMP clamp );

	static E_TEXTURE_CLAMP getTextureAddressMode(D3D11_TEXTURE_ADDRESS_MODE address);

	static DXGI_FORMAT getD3DIndexType(E_INDEX_TYPE type);

	static D3D11_PRIMITIVE_TOPOLOGY getD3DTopology(E_PRIMITIVE_TYPE type);
};

inline ECOLOR_FORMAT CD3D11Helper::getColorFormatFromDXGIFormat( DXGI_FORMAT format )
{
	switch(format)
	{
	case DXGI_FORMAT_A8_UNORM:
		return ECF_A8;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return ECF_A8R8G8B8;
	case DXGI_FORMAT_B5G6R5_UNORM:
		return ECF_R5G6B5;
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return ECF_A1R5G5B5;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return ECF_ARGB32F;

	case DXGI_FORMAT_D16_UNORM:
		return ECF_D16;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return ECF_D24S8;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return ECF_DXT1;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		return ECF_DXT3;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		return ECF_DXT5;
	default:
		ASSERT(false);
	}	
	return (ECOLOR_FORMAT)ECF_UNKNOWN;
}

inline DXGI_FORMAT CD3D11Helper::getDXGIFormatFromColorFormat( ECOLOR_FORMAT format )
{
	switch(format)
	{
	case ECF_A8:
		return DXGI_FORMAT_A8_UNORM;			
	case ECF_A8R8G8B8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case ECF_R5G6B5:
		return DXGI_FORMAT_B5G6R5_UNORM;
	case ECF_A1R5G5B5:
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	case ECF_ARGB32F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case ECF_D16:
		return DXGI_FORMAT_D16_UNORM;
	case ECF_D24S8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;

	case ECF_DXT1:
		return DXGI_FORMAT_BC1_UNORM;
	case ECF_DXT3:
		return DXGI_FORMAT_BC2_UNORM;
	case ECF_DXT5:
		return DXGI_FORMAT_BC3_UNORM;
	default:
		ASSERT(false);
	}
	return DXGI_FORMAT_UNKNOWN;
}

inline DXGI_FORMAT CD3D11Helper::getDXGIFormatFromColorFormat(ECOLOR_FORMAT format, DXGI_FORMAT& depthTextureFmt, DXGI_FORMAT& depthSRFmt)
{
	switch(format)
	{
	case ECF_D16:
		depthTextureFmt = DXGI_FORMAT_R16_TYPELESS;
		depthSRFmt = DXGI_FORMAT_R16_UNORM;
		return DXGI_FORMAT_D16_UNORM;
	case ECF_D24S8:
		depthTextureFmt = DXGI_FORMAT_R24G8_TYPELESS;
		depthSRFmt = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	default:
		ASSERT(false);
	}
	return DXGI_FORMAT_UNKNOWN;
}

inline D3D11_COMPARISON_FUNC CD3D11Helper::getD3DCompare( E_COMPARISON_FUNC comp )
{
	D3D11_COMPARISON_FUNC r = D3D11_COMPARISON_NEVER;
	switch(comp)
	{
	case ECFN_NEVER:
		r = D3D11_COMPARISON_NEVER; break;
	case	ECFN_LESSEQUAL:
		r = D3D11_COMPARISON_LESS_EQUAL; break;
	case	ECFN_EQUAL:
		r = D3D11_COMPARISON_EQUAL; break;
	case 	ECFN_LESS:
		r = D3D11_COMPARISON_LESS; break;
	case	ECFN_NOTEQUAL:
		r = D3D11_COMPARISON_NOT_EQUAL; break;
	case	ECFN_GREATEREQUAL:
		r = D3D11_COMPARISON_GREATER_EQUAL; break;
	case	ECFN_GREATER:
		r = D3D11_COMPARISON_GREATER;	break;
	case	ECFN_ALWAYS:
		r = D3D11_COMPARISON_ALWAYS;	break;
	default:
		ASSERT(false);
	}
	return r;
}

inline const c8* CD3D11Helper::getD3DFormatString( DXGI_FORMAT format )
{
	switch(format)
	{
	case DXGI_FORMAT_A8_UNORM:
		return "A8";
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return "B8G8R8A8";
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return "R8G8B8A8";
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return "BC1";
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		return "BC2";
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		return "BC3";
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		return "D32S8X24";
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return "D24S8";
	default:
		return "Unknown";
	}	
}

inline D3D11_BLEND CD3D11Helper::getD3DBlend( E_BLEND_FACTOR factor )
{
	D3D11_BLEND r = D3D11_BLEND_ZERO;
	switch ( factor )
	{
	case EBF_ZERO:	
		r = D3D11_BLEND_ZERO; break;
	case EBF_ONE:	
		r = D3D11_BLEND_ONE; break;
	case EBF_DST_COLOR:	
		r = D3D11_BLEND_DEST_COLOR; break;
	case EBF_ONE_MINUS_DST_COLOR:
		r = D3D11_BLEND_INV_DEST_COLOR; break;
	case EBF_SRC_COLOR:		
		r = D3D11_BLEND_SRC_COLOR; break;
	case EBF_ONE_MINUS_SRC_COLOR:
		r = D3D11_BLEND_INV_SRC_COLOR; break;
	case EBF_SRC_ALPHA:	
		r = D3D11_BLEND_SRC_ALPHA; break;
	case EBF_ONE_MINUS_SRC_ALPHA:
		r = D3D11_BLEND_INV_SRC_ALPHA; break;
	case EBF_DST_ALPHA:	
		r = D3D11_BLEND_DEST_ALPHA; break;
	case EBF_ONE_MINUS_DST_ALPHA:
		r = D3D11_BLEND_INV_DEST_ALPHA; break;
	case EBF_SRC_ALPHA_SATURATE:
		r = D3D11_BLEND_SRC_ALPHA_SAT; break;
	default:
		ASSERT(false);
	}
	return r;
}

inline D3D11_BLEND_OP CD3D11Helper::getD3DBlendOp( E_BLEND_OP op )
{
	D3D11_BLEND_OP r = D3D11_BLEND_OP_ADD;
	switch(op)
	{
	case EBO_ADD:
		r = D3D11_BLEND_OP_ADD;
		break;
	case EBO_SUBSTRACT:
		r = D3D11_BLEND_OP_SUBTRACT;
		break;
	default:
		ASSERT(false);
	}
	return r;
}

inline D3D11_TEXTURE_ADDRESS_MODE CD3D11Helper::getD3DTextureAddress( E_TEXTURE_CLAMP clamp )
{
	switch (clamp)
	{
	case ETC_REPEAT:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	case ETC_CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	case ETC_MIRROR:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case ETC_CLAMP_TO_BORDER:
		return D3D11_TEXTURE_ADDRESS_BORDER;
	case ETC_MIRROR_CLAMP:
	case ETC_MIRROR_CLAMP_TO_BORDER:
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	default:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	}
}

inline E_TEXTURE_CLAMP CD3D11Helper::getTextureAddressMode( D3D11_TEXTURE_ADDRESS_MODE address )
{
	switch(address)
	{
	case D3D11_TEXTURE_ADDRESS_WRAP:
		return ETC_REPEAT;
	case D3D11_TEXTURE_ADDRESS_CLAMP:
		return ETC_CLAMP;
	case D3D11_TEXTURE_ADDRESS_MIRROR:
		return ETC_MIRROR;
	case D3D11_TEXTURE_ADDRESS_BORDER:
		return ETC_CLAMP_TO_BORDER;
	case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
		return ETC_MIRROR_CLAMP;
	default:
		return ETC_REPEAT;
	}
}

inline DXGI_FORMAT CD3D11Helper::getD3DIndexType( E_INDEX_TYPE type )
{
	switch(type)
	{
	case EIT_16BIT:
		return DXGI_FORMAT_R16_UINT;
	case EIT_32BIT:
		return DXGI_FORMAT_R32_UINT;
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

inline D3D11_PRIMITIVE_TOPOLOGY CD3D11Helper::getD3DTopology( E_PRIMITIVE_TYPE type )
{
	switch (type)
	{
	case EPT_POINTS:
		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case EPT_LINE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case EPT_LINES:
		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case EPT_TRIANGLE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case EPT_TRIANGLES:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	default:
		ASSERT(false);
		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

#endif