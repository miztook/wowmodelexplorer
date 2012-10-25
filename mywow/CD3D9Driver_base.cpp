#include "stdafx.h"
#include "CD3D9Driver.h"
#include "mywow.h"
#include "CD3D9Helper.h"
#include "CD3D9RenderTarget.h"
#include "CD3D9Shader.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9MaterialRenderServices.h"
#include "CD3D9SceneStateServices.h"

#define  DEVICE_SET_RENDER_STATE(prop, d3drs, v)	if (SSCache.##prop != v)		\
		{	pID3DDevice->SetRenderState(d3drs, v);				\
		SSCache.##prop = v;	} 

typedef IDirect3D9* (__stdcall *D3DCREATETYPE)(UINT);
typedef HRESULT (__stdcall *D3DCREATETYPEEX)(UINT, IDirect3D9Ex**);

//vertex declaration
//顶点使用多流系统以减少传输带宽
/*
	G:		pos, normal, color0(diffuse), color1(specular)
	T:		texturecoord, tanget, binormal
	A:		blend weight, blend indices
	I:		instancing
*/

D3DVERTEXELEMENT9 declBasicColor[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color

	D3DDECL_END()
};

D3DVERTEXELEMENT9 declBasicTex_SingleStream[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	{0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},				//tex

	D3DDECL_END()
};

D3DVERTEXELEMENT9 declBasicTex_MultiStream[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	{1, 0, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},				//tex

	D3DDECL_END()
};

D3DVERTEXELEMENT9 declStandard_SingleStream[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},					//normal
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color
	{0, 28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 declStandard_MultiStream[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},					//normal
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color
	{1, 0, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 decl2TCoords_SingleStream[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},					//normal
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color
	{0, 28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 decl2TCoords_MultiStream[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},					//normal
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color
	{1, 0, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{1, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 declTangent_MultiStream[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},					//normal
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color
	{1, 0, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{1, 8, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
	{1, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
	D3DDECL_END()
};

static D3DVERTEXELEMENT9 declVertexModel_MultiStream[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{2, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
	{2, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES,0},
	D3DDECL_END()
};

CD3D9Driver::CD3D9Driver()
{
	HLib = NULL;
	pID3D = NULL;
	pID3DDevice = NULL;

	HWnd = 0;
	DevType = D3DDEVTYPE_HAL;

	DeviceLost = false;

	DefaultDepthBuffer = NULL;
	DefaultBackBuffer = NULL;
	BackBufferFormat = D3DFMT_UNKNOWN;

	PrimitivesDrawn = 0;
	DrawCall = 0;
	AlphaToCoverageSupport = false;

	CurrentDeviceState.reset();
	CurrentRenderMode = ERM_NONE;
	ResetRenderStates = true;
	CurrentRenderTarget = NULL;

	MaterialRenderer = NULL;
	ShaderServices = NULL;
	MaterialRenderServices = NULL;
	SceneStateServices = NULL;

	AdapterCount = 0;

	//2D
	InitMaterial2D.MaterialType = EMT_2D;
	InitMaterial2D.Lighting = false;
	InitMaterial2D.ZWriteEnable = false;
	InitMaterial2D.ZBuffer = ECFN_NEVER;
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		InitMaterial2D.TextureLayer[i].TextureWrapU=ETC_REPEAT;
		InitMaterial2D.TextureLayer[i].TextureWrapV=ETC_REPEAT;
		InitMaterial2D.TextureLayer[i].UseTextureMatrix = false;

		InitOverrideMaterial2D.TextureFilters[i] = ETF_NONE;
	}

	memset(VertexDeclarations, 0, sizeof(VertexDeclarations));
}

CD3D9Driver::~CD3D9Driver()
{
	releaseVertexDecl();

	delete		SceneStateServices;
	delete		MaterialRenderServices;
	delete		ShaderServices;

	SAFE_RELEASE(DefaultBackBuffer);
	SAFE_RELEASE(DefaultDepthBuffer);

	if(pID3DDevice)
		pID3DDevice->Release();
	if(pID3D)
		pID3D->Release();

	if (HLib)
		FreeLibrary(HLib);
}

bool CD3D9Driver::initDriver( HWND hwnd, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{
	_ASSERT(::IsWindow(hwnd));

	HWnd = hwnd;
	DriverSetting.vsync = vsync;
	DriverSetting.fullscreen = fullscreen;

	RECT rc;
	::GetClientRect(hwnd, &rc);
	dimension2du windowSize((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);

	HLib = ::LoadLibraryA("d3d9.dll");
	if (!HLib)
	{
		_ASSERT(false);
		return false;
	}

	HRESULT hr;

	D3DCREATETYPE d3dcreate = (D3DCREATETYPE)::GetProcAddress(HLib, "Direct3DCreate9");
	if(!d3dcreate) { _ASSERT(false); return false; }
	pID3D = (d3dcreate)(D3D_SDK_VERSION);
	if (!pID3D) { _ASSERT(false); return false; }
	
	//device info
	AdapterCount = pID3D->GetAdapterCount();
	AdapterInfo.index = (adapter >= AdapterCount ? 0 : adapter);

	D3DADAPTER_IDENTIFIER9 identifier;
	if (FAILED(pID3D->GetAdapterIdentifier(adapter, 0, &identifier)))
	{
		AdapterInfo.index = D3DADAPTER_DEFAULT;
		if(FAILED(pID3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &identifier )))
		{
			_ASSERT(false);
			return false;
		}
	}
	
	strcpy_s( AdapterInfo.description, 512,  identifier.Description );
	strcpy_s( AdapterInfo.name, 32,  identifier.DeviceName );

	AdapterInfo.vendorID = identifier.VendorId;
	switch (identifier.VendorId)
	{
	case 0x1002: strcpy_s(AdapterInfo.vendorName, DEFAULT_SIZE, "ATI Technologies Inc."); break;
	case 0x10DE: strcpy_s(AdapterInfo.vendorName, DEFAULT_SIZE, "NVIDIA Corporation"); break;
	case 0x102B: strcpy_s(AdapterInfo.vendorName, DEFAULT_SIZE, "Matrox Electronic System Ltd."); break;
	case 0x121A: strcpy_s(AdapterInfo.vendorName, DEFAULT_SIZE, "3dfx Interactive Inc"); break;
	case 0x5333: strcpy_s(AdapterInfo.vendorName, DEFAULT_SIZE, "S3 Graphics Co. Ltd"); break;
	case 0x8086: strcpy_s(AdapterInfo.vendorName, DEFAULT_SIZE, "Intel Corporation"); break;
	default:
		sprintf_s(AdapterInfo.vendorName, DEFAULT_SIZE, "Unknown VendorId: %x", identifier.VendorId);
		break;
	}

	// set present params...
	D3DDISPLAYMODE d3ddm;
	pID3D->GetAdapterDisplayMode(AdapterInfo.index, &d3ddm);
	u32 numModes = pID3D->GetAdapterModeCount(AdapterInfo.index, d3ddm.Format);
	for (u32 i=0; i<numModes; ++i)
	{
		D3DDISPLAYMODE ddm;
		pID3D->EnumAdapterModes(AdapterInfo.index, d3ddm.Format, i, &ddm);

		SDisplayMode displayMode;
		displayMode.width = (u16)ddm.Width;
		displayMode.height = (u16)ddm.Height;
		displayMode.refreshrate = (u8)ddm.RefreshRate;
		AdapterInfo.displayModes.push_back(displayMode);
	}

	ZeroMemory(&present, sizeof(present));

	present.BackBufferCount		= 1;
	present.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	if (vsync)
		present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	else
		present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (DriverSetting.fullscreen)
	{
		present.BackBufferWidth = /*::GetSystemMetrics(SM_CXSCREEN);*/windowSize.Width;
		present.BackBufferHeight =/*::GetSystemMetrics(SM_CYSCREEN);*/ windowSize.Height;
		present.BackBufferFormat = d3ddm.Format;
		present.SwapEffect	= D3DSWAPEFFECT_FLIP;
		present.Windowed	= FALSE;
		present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}
	else
	{
		present.BackBufferWidth = windowSize.Width;
		present.BackBufferHeight =windowSize.Height;
		present.BackBufferFormat = d3ddm.Format;
		present.SwapEffect		= D3DSWAPEFFECT_DISCARD;
		present.Windowed		= TRUE;
		present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}

	//antialias
	if (antialias > 0)
	{
		if(antialias > 16)
			antialias = 16;

		DWORD qualityLevels = 0;

		while(antialias > 0)
		{
			if(SUCCEEDED(pID3D->CheckDeviceMultiSampleType(AdapterInfo.index,
				DevType, present.BackBufferFormat, !fullscreen,
				(D3DMULTISAMPLE_TYPE)antialias, &qualityLevels)))
			{
				u32 quality = 0; //min_((u32)(qualityLevels-1), 1u);
				present.MultiSampleType	= (D3DMULTISAMPLE_TYPE)antialias;
				present.MultiSampleQuality = quality;
				break;
			}
			--antialias;
		}
	}
	DriverSetting.antialias = antialias;

	//depth stencil
	present.AutoDepthStencilFormat = D3DFMT_D24X8;
	if(FAILED(pID3D->CheckDeviceFormat(AdapterInfo.index, DevType,
		present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, present.AutoDepthStencilFormat)))
	{
		present.AutoDepthStencilFormat = D3DFMT_D24S8;
		if(FAILED(pID3D->CheckDeviceFormat(AdapterInfo.index, DevType,
			present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, present.AutoDepthStencilFormat)))
		{
			present.AutoDepthStencilFormat = D3DFMT_D15S1;
			if(FAILED(pID3D->CheckDeviceFormat(AdapterInfo.index, DevType,
				present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, present.AutoDepthStencilFormat)))
			{
				_ASSERT(false);
			}
		}
	}
	else
		if(FAILED(pID3D->CheckDepthStencilMatch(AdapterInfo.index, DevType,
			present.BackBufferFormat, present.BackBufferFormat, present.AutoDepthStencilFormat)))
		{
			_ASSERT(false);
		}

	present.EnableAutoDepthStencil	= TRUE;

	//create device
	DWORD creationFlag = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
	if (multithread)
		creationFlag |= D3DCREATE_MULTITHREADED;

	hr = pID3D->CreateDevice(AdapterInfo.index, DevType, hwnd, creationFlag, &present, &pID3DDevice);

	_ASSERT(SUCCEEDED(hr));				//强制hardware processing

	if (!pID3DDevice) { _ASSERT(false); return false; }

	//get caps
	pID3DDevice->GetDeviceCaps(&Caps);
	if (Caps.MaxVertexShaderConst < 256 ||
		!queryFeature(EVDF_STREAM_OFFSET) ||
		!queryFeature(EVDF_RENDER_TO_TARGET) ||
		!queryFeature(EVDF_MIP_MAP) ||
		!queryFeature(EVDF_TEXTURE_ADDRESS) ||
		!queryFeature(EVDF_SEPARATE_UVWRAP))
	{
		MessageBoxA(NULL, "显卡不支持某些功能，设备不能正常运行!", "警告", MB_ICONEXCLAMATION);
		return false;
	}

	switch(AdapterInfo.vendorID)
	{
	case 0x10DE:		//NVidia
		AlphaToCoverageSupport = (pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			D3DFMT_X8R8G8B8, 0,D3DRTYPE_SURFACE,
			(D3DFORMAT)MAKEFOURCC('A', 'T', 'O', 'C')) == S_OK);
		break;
	case 0x1002:		//ATI
		AlphaToCoverageSupport = (pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			D3DFMT_X8R8G8B8, 0,D3DRTYPE_SURFACE,
			(D3DFORMAT)MAKEFOURCC('A','2','M','1')) == S_OK);
		break;
	}

	//depth buffer
	pID3DDevice->GetDepthStencilSurface(&DefaultDepthBuffer);
	
	//back buffer format
	BackBufferFormat = D3DFMT_A8R8G8B8;
	if (SUCCEEDED(pID3DDevice->GetRenderTarget(0, &DefaultBackBuffer)))
	//if (SUCCEEDED(pID3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &DefaultBackBuffer)))
	{
		D3DSURFACE_DESC desc;
		DefaultBackBuffer->GetDesc(&desc);
		BackBufferFormat = desc.Format;

		if (BackBufferFormat == D3DFMT_X8R8G8B8)
			BackBufferFormat = D3DFMT_A8R8G8B8;
	}
	else
	{
		_ASSERT(false);
		return false;
	}

	//shader, material
	ShaderServices = new CD3D9ShaderServices;
	MaterialRenderServices = new CD3D9MaterialRenderServices(true);//(queryFeature(EVDF_PIXEL_SHADER_1_1));
	SceneStateServices = new CD3D9SceneStateServices;

	createVertexDecl();	
	
	setDisplayMode(windowSize);

	return true;
}

bool CD3D9Driver::beginScene()
{
	HRESULT hr;

	PrimitivesDrawn = 0;
	DrawCall = 0;

	if (DeviceLost)
	{
		if (FAILED(hr = pID3DDevice->TestCooperativeLevel()))
		{
			if (hr == D3DERR_DEVICELOST)
			{
				Sleep(100);
				hr = pID3DDevice->TestCooperativeLevel();
				if (hr == D3DERR_DEVICELOST)
					return false;
			}
			if ( hr == D3DERR_DEVICENOTRESET &&
				!reset() )
				return false;
		}
	}

	hr = pID3DDevice->BeginScene();
	if ( FAILED(hr) )
	{
		if( hr == D3DERR_DEVICELOST )
			DeviceLost = true;
		return false;
	}
	return true;
}

bool CD3D9Driver::endScene()
{
	pID3DDevice->EndScene();

	HRESULT hr = pID3DDevice->Present(NULL, NULL, NULL, NULL);

	if (SUCCEEDED(hr))
		return true;

	if( hr == D3DERR_DEVICELOST )
	{
		DeviceLost = true;
	}

	return false;
}

bool CD3D9Driver::clear( bool renderTarget, bool zBuffer, bool stencil, SColor color )
{
	HRESULT hr;
	DWORD flags = 0;

	if (DeviceLost)
	{
		if (FAILED(hr = pID3DDevice->TestCooperativeLevel()))
		{
			if (hr == D3DERR_DEVICELOST)
			{
				Sleep(100);
				hr = pID3DDevice->TestCooperativeLevel();
				if (hr == D3DERR_DEVICELOST)
					return false;
			}
			if ( hr == D3DERR_DEVICENOTRESET &&
				!reset() )
				return false;
		}
	}

	if (renderTarget)
		flags |= D3DCLEAR_TARGET;

	if (zBuffer)
		flags |= D3DCLEAR_ZBUFFER;
		

	if (stencil)
		flags |= D3DCLEAR_STENCIL;

	if (flags)
	{
		hr = pID3DDevice->Clear( 0, NULL, flags, color.color, 1.0f, 0);
		if (FAILED(hr)) { _ASSERT(false); return false; }
	}
	return true;
}

bool CD3D9Driver::queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
{
	switch (feature)
	{
	case EVDF_RENDER_TO_TARGET:
		return Caps.NumSimultaneousRTs > 0;
	case EVDF_HARDWARE_TL:
		return (Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
	case EVDF_TEXTURE_ADDRESS:
		return (Caps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP) &&
			(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR) &&
			(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER) &&
			(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE);
	case EVDF_SEPARATE_UVWRAP:
		return (Caps.TextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV) != 0;
	case EVDF_MIP_MAP:
		return (Caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) != 0;
	case EVDF_STENCIL_BUFFER:
		return Caps.StencilCaps != 0;
	case EVDF_VERTEX_SHADER_1_1:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(1,1);
	case EVDF_VERTEX_SHADER_2_0:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(2,0);
	case EVDF_VERTEX_SHADER_3_0:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(3,0);
	case EVDF_PIXEL_SHADER_1_1:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(1,1);
	case EVDF_PIXEL_SHADER_2_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(2,0);
	case EVDF_PIXEL_SHADER_3_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(3,0);
	case EVDF_TEXTURE_NSQUARE:
		return (Caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) == 0;
	case EVDF_TEXTURE_NPOT:
		return (Caps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0;
	case EVDF_COLOR_MASK:
		return (Caps.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE) != 0;
	case EVDF_MULTIPLE_RENDER_TARGETS:
		return Caps.NumSimultaneousRTs > 1;
	case EVDF_MRT_COLOR_MASK:
		return (Caps.PrimitiveMiscCaps & D3DPMISCCAPS_INDEPENDENTWRITEMASKS) != 0;
	case EVDF_MRT_BLEND:
		return (Caps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING) != 0;
	case EVDF_STREAM_OFFSET:
		return (Caps.DevCaps2 & D3DDEVCAPS2_STREAMOFFSET ) != 0;
	case EVDF_W_BUFFER:
		return (Caps.RasterCaps & D3DPRASTERCAPS_WBUFFER) != 0;
	default:
		return false;
	};
}

void CD3D9Driver::setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat )
{
	_ASSERT( 0 <= state && state < ETS_COUNT );

	switch( state )
	{
	case ETS_VIEW:
		if (CurrentDeviceState.matrices[ETS_VIEW] != mat)
		{
			pID3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)(void*)mat.pointer());
			CurrentDeviceState.matrices[ETS_VIEW] = mat;
		}
		break;
	case ETS_WORLD:
		if (CurrentDeviceState.matrices[ETS_WORLD] != mat)
		{
			pID3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(void*)mat.pointer());
			CurrentDeviceState.matrices[ETS_WORLD] = mat;
		}
		break;
	case ETS_PROJECTION:
		if (CurrentDeviceState.matrices[ETS_PROJECTION] != mat)
		{
			pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)(void*)mat.pointer());
			CurrentDeviceState.matrices[ETS_PROJECTION] = mat;
		}	
		break;
	default:		//texture
		if ( state - ETS_TEXTURE_0 < MATERIAL_MAX_TEXTURES )
		{
			if (CurrentDeviceState.matrices[state - ETS_TEXTURE_0] != mat)
			{
				pID3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+ ( state - ETS_TEXTURE_0 )),
					(D3DMATRIX*)((void*)mat.pointer()));

				CurrentDeviceState.matrices[state - ETS_TEXTURE_0] = mat;
			}
		}
		break;
	}
}

void CD3D9Driver::setMaterial(const SMaterial& material)
{
	Material = material; 
}

void CD3D9Driver::setTexture( u32 stage, ITexture* texture )
{
	MaterialRenderServices->setSampler_Texture(stage, texture);
}

ITexture* CD3D9Driver::getTexture( u32 index ) const
{
	return MaterialRenderServices->getSampler_Texture(index);
}

bool CD3D9Driver::setRenderTarget( IRenderTarget* texture )
{
	CD3D9RenderTarget* tex = static_cast<CD3D9RenderTarget*>(texture);

	if ( tex == NULL )
	{
		if (FAILED(pID3DDevice->SetRenderTarget(0, DefaultBackBuffer)))
		{
			_ASSERT(false);
			return false;
		}
		if (FAILED(pID3DDevice->SetDepthStencilSurface(DefaultDepthBuffer)))
		{
			_ASSERT(false);
			return false;
		}
	}
	else
	{
		if (FAILED(pID3DDevice->SetRenderTarget(0, tex->getRTTSurface())))
		{
			_ASSERT(false);
			return false;
		}

		if (FAILED(pID3DDevice->SetDepthStencilSurface(tex->getRTTDepthSurface())))
		{
			_ASSERT(false);
			return false;
		}
	}

	CurrentRenderTarget = texture;

	return true;
}

void CD3D9Driver::setViewPort( recti area )
{
	recti vp = area;
	vp.clipAgainst(recti(0,0,ScreenSize.Width,ScreenSize.Height));
	if ( vp.getWidth() < 0 || vp.getHeight() < 0 )
		return;

	D3DVIEWPORT9 viewPort;
	viewPort.X = vp.UpperLeftCorner.X;
	viewPort.Y = vp.UpperLeftCorner.Y;
	viewPort.Width = vp.getWidth();
	viewPort.Height = vp.getHeight();
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;

	HRESULT hr = pID3DDevice->SetViewport(&viewPort);
	if(SUCCEEDED(hr))
		Viewport = vp;
}

void CD3D9Driver::setDisplayMode( const dimension2du& size )
{
	ScreenSize = size;

	setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));

	present.BackBufferWidth = ScreenSize.Width;
	present.BackBufferHeight = ScreenSize.Height;

	reset();
}

dimension2du CD3D9Driver::getDisplayMode() const
{
	return dimension2du(present.BackBufferWidth, present.BackBufferHeight);
}

bool CD3D9Driver::setDriverSetting( const SDriverSetting& setting )
{
	bool change = false;
	bool vsync = setting.vsync;
	u8 antialias = setting.antialias;
	bool fullscreen = setting.fullscreen;

	if (vsync != DriverSetting.vsync)
	{
		present.PresentationInterval = vsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
		change = true;
	}

	if (antialias > 0)
	{
		if(antialias > 16)
			antialias = 16;

		DWORD qualityLevels = 0;

		while(antialias > 0)
		{
			if(SUCCEEDED(pID3D->CheckDeviceMultiSampleType(AdapterInfo.index,
				DevType, present.BackBufferFormat, !fullscreen,
				(D3DMULTISAMPLE_TYPE)antialias, &qualityLevels)))
			{
				break;
			}
			--antialias;
		}
	}

	if (antialias != DriverSetting.antialias)
	{
		present.MultiSampleType = (D3DMULTISAMPLE_TYPE)antialias;
		change = true;
	}

	if (fullscreen != DriverSetting.fullscreen)
	{
		RECT rc;
		::GetClientRect(HWnd, &rc);
		dimension2du windowSize((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);
		D3DDISPLAYMODE d3ddm;
		pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

		if (fullscreen)
		{
			present.BackBufferWidth = /*::GetSystemMetrics(SM_CXSCREEN);*/windowSize.Width;
			present.BackBufferHeight =/*::GetSystemMetrics(SM_CYSCREEN);*/ windowSize.Height;
			present.BackBufferFormat = d3ddm.Format;
			present.SwapEffect	= D3DSWAPEFFECT_FLIP;
			present.Windowed	= FALSE;
			present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		}
		else
		{
			present.BackBufferWidth = windowSize.Width;
			present.BackBufferHeight =windowSize.Height;
			present.BackBufferFormat = d3ddm.Format;
			present.SwapEffect		= D3DSWAPEFFECT_DISCARD;
			present.Windowed		= TRUE;
			present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		}

		ScreenSize = windowSize;
	}

	bool ret = true;
	if (change)
	{
// 		if (beginScene())
// 		{
// 			clear(true, false, false, SColor(0,0,0));
// 			endScene();
// 		}
		setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));
		ret = reset();

		if(ret)
		{ 
			DriverSetting = setting;
		}
	}
	return ret;
}

bool CD3D9Driver::reset()
{
	for( T_LostResetList::iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onLost();

	//depth stencil buffer
	SAFE_RELEASE(DefaultDepthBuffer);

	//back buffer
	SAFE_RELEASE(DefaultBackBuffer);

	HRESULT hr;
	hr = pID3DDevice->Reset(&present);

	if ( FAILED(hr) )
	{
		if (hr == D3DERR_DEVICELOST)
			DeviceLost = true;
		else
		{
			//	_ASSERT(hr == D3DERR_DRIVERINTERNALERROR);
			_ASSERT(false);					//有资源没有lost, reset
		}

		return false;
	}

	//back buffer format
	BackBufferFormat = D3DFMT_A8R8G8B8;
	//if (SUCCEEDED(pID3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &DefaultBackBuffer)))
	if (SUCCEEDED(pID3DDevice->GetRenderTarget(0, &DefaultBackBuffer)))
	{
		D3DSURFACE_DESC desc;
		DefaultBackBuffer->GetDesc(&desc);
		BackBufferFormat = desc.Format;

		if (BackBufferFormat == D3DFMT_X8R8G8B8)
			BackBufferFormat = D3DFMT_A8R8G8B8;
	}

	//depth buffer size
	pID3DDevice->GetDepthStencilSurface(&DefaultDepthBuffer);		
	for( T_LostResetList::iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onReset();

	DeviceLost = false;

	//reset
	LastMaterial.MaterialType = (E_MATERIAL_TYPE)-1;
	ResetRenderStates = true;
	CurrentRenderMode = ERM_NONE;
	CurrentDeviceState.reset();
	
	SceneStateServices->reset();

	return true;
}

void CD3D9Driver::createVertexDecl()
{
	//vertex declation
	pID3DDevice->CreateVertexDeclaration(declBasicColor, &VertexDeclarations[EVT_BASICCOLOR]);
	pID3DDevice->CreateVertexDeclaration(declBasicTex_SingleStream, &VertexDeclarations[EVT_BASICTEX_S]);
	pID3DDevice->CreateVertexDeclaration(declBasicTex_MultiStream, &VertexDeclarations[EVT_BASICTEX_M]);
	pID3DDevice->CreateVertexDeclaration(declStandard_MultiStream, &VertexDeclarations[EVT_STANDARD]);
	pID3DDevice->CreateVertexDeclaration(decl2TCoords_MultiStream, &VertexDeclarations[EVT_2TCOORDS]);
	pID3DDevice->CreateVertexDeclaration(declTangent_MultiStream, &VertexDeclarations[EVT_TANGENTS]);
	pID3DDevice->CreateVertexDeclaration(declVertexModel_MultiStream, &VertexDeclarations[EVT_MODEL]);

}

void CD3D9Driver::releaseVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		SAFE_RELEASE(VertexDeclarations[i]);
	}
}

IDirect3DVertexDeclaration9* CD3D9Driver::getD3DVertexDeclaration( E_VERTEX_TYPE type )
{
	return VertexDeclarations[type];
}

void CD3D9Driver::setVertexDeclaration( E_VERTEX_TYPE type )
{
	if (CurrentDeviceState.vType != type)
	{
		IDirect3DVertexDeclaration9* decl = getD3DVertexDeclaration(type);
		_ASSERT(decl);
		pID3DDevice->SetVertexDeclaration(decl);
		CurrentDeviceState.vType = type;
	}
}

