#include "stdafx.h"
#include "CD3D9Helper.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Driver.h"

bool CD3D9Helper::init()
{
	return true;
}

const c8* CD3D9Helper::getD3DFormatString( D3DFORMAT format )
{
	switch(format)
	{
	case D3DFMT_A8:
		return "A8";
	case D3DFMT_A8L8:
		return "A8L8";
	case D3DFMT_X1R5G5B5:
		return "X1R5G5B5";
	case D3DFMT_A1R5G5B5:
		return "A1R5G5B5";
	case D3DFMT_A8R8G8B8:
		return "A8R8G8B8";
	case D3DFMT_X8R8G8B8:
		return "X8R8G8B8";
	case D3DFMT_R5G6B5:
		return "R5G6B5";
	case D3DFMT_R8G8B8:
		return "R8G8B8";
	case D3DFMT_DXT1:
		return "DXT1";
	case D3DFMT_DXT3:
		return "DXT3";
	case D3DFMT_DXT5:
		return "DXT5";
	case D3DFMT_D24X8:
		return "D24X8";
	case D3DFMT_D24S8:
		return "D24S8";
	case D3DFMT_D16:
		return "D16";
	case D3DFMT_D32:
		return "D32";
	case D3DFMT_D15S1:
		return "D15S1";
	case D3DFMT_D24X4S4:
		return "D24X4S4";
	default:
		return "Unknown";
	};
}

#endif