#include "stdafx.h"
#include "CD3D9Driver.h"
#include "mywow.h"
#include "CD3D9Helper.h"
#include "CD3D9Texture.h"
#include "CD3D9Shader.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9MaterialRenderer.h"
#include "CD3D9MaterialRenderServices.h"

#define  DEVICE_SET_RENDER_STATE(prop, d3drs, v)	if (SSCache.##prop != v)		\
		{	pID3DDevice->SetRenderState(d3drs, v);				\
		SSCache.##prop = v;	} 

//vertex declaration
D3DVERTEXELEMENT9 declBasic[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position

	D3DDECL_END()
};


D3DVERTEXELEMENT9 declBasicColor[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color

	D3DDECL_END()
};

D3DVERTEXELEMENT9 declBasicTex[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	{0, 16, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},				//tex

	D3DDECL_END()
};

D3DVERTEXELEMENT9 declStandard[] = 
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},					//normal
	{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},					//color
	{0, 28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

static D3DVERTEXELEMENT9 declVertexModel[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},					//position
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,0},
	{1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES,0},
	D3DDECL_END()
};

CD3D9Driver::CD3D9Driver()
{
	HLib = NULL;
	pID3D = NULL;
	pID3DDevice = NULL;

	HWnd = 0;
	Adapter = 0;
	DevType = (D3DDEVTYPE)0;
	VSync = false;
	FullScreen = false;
	AntiAliasing = 0;
	Quality = 0;

	DeviceLost = false;

	DepthStencilSurface = NULL;
	DepthStencilSize.set(0,0);
	BackBuffer = NULL;
	BackBufferFormat = D3DFMT_UNKNOWN;

	PrimitivesDrawn = 0;
	DrawCall = 0;
	MaxTextureUnits = 0;
	MaxUserClipPlanes = 0;
	VendorID = 0;
	AlphaToCoverageSupport = false;
	CSAAApplied = false;

	CurrentDeviceState.reset();
	CurrentRenderMode = ERM_NONE;
	ResetRenderStates = true;
	PrevRenderTarget = NULL;

	MaterialRenderer = NULL;
	ShaderServices = NULL;
	MaterialRenderServices = NULL;

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
	}

	memset(VertexDeclarations, 0, sizeof(VertexDeclarations));
}

CD3D9Driver::~CD3D9Driver()
{
	releaseVertexDecl();

	delete		MaterialRenderServices;
	delete		ShaderServices;

	SAFE_RELEASE(BackBuffer);
	SAFE_RELEASE(DepthStencilSurface);

	if(pID3DDevice)
		pID3DDevice->Release();
	if(pID3D)
		pID3D->Release();

	if (HLib)
		FreeLibrary(HLib);
}

bool CD3D9Driver::initDriver( HWND hwnd, bool fullscreen, bool vsync, bool multithread, u32 bits, u8 antialias, E_CSAA csaa )
{
	_ASSERT(::IsWindow(hwnd));

	HWnd = hwnd;
	VSync = vsync;
	FullScreen = fullscreen;

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
	D3DADAPTER_IDENTIFIER9 identifier;
	pID3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &identifier );

	strcpy_s( DeviceDescription, 512,  identifier.Description );
	strcpy_s( DeviceName, 32,  identifier.DeviceName );

	VendorID = identifier.VendorId;
	switch (identifier.VendorId)
	{
	case 0x1002: strcpy_s(VendorName, DEFAULT_SIZE, "ATI Technologies Inc."); break;
	case 0x10DE: strcpy_s(VendorName, DEFAULT_SIZE, "NVIDIA Corporation"); break;
	case 0x102B: strcpy_s(VendorName, DEFAULT_SIZE, "Matrox Electronic System Ltd."); break;
	case 0x121A: strcpy_s(VendorName, DEFAULT_SIZE, "3dfx Interactive Inc"); break;
	case 0x5333: strcpy_s(VendorName, DEFAULT_SIZE, "S3 Graphics Co. Ltd"); break;
	case 0x8086: strcpy_s(VendorName, DEFAULT_SIZE, "Intel Corporation"); break;
	default:
		sprintf_s(VendorName, DEFAULT_SIZE, "Unknown VendorId: %x", identifier.VendorId);
		break;
	}

	// set present params...
	D3DDISPLAYMODE d3ddm;
	pID3D->GetAdapterDisplayMode(0, &d3ddm);

	ZeroMemory(&present, sizeof(present));

	present.BackBufferCount		= 1;
	present.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	if (vsync)
		present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	else
		present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (fullscreen)
	{
		present.BackBufferWidth = /*::GetSystemMetrics(SM_CXSCREEN);*/windowSize.Width;
		present.BackBufferHeight =/*::GetSystemMetrics(SM_CYSCREEN);*/ windowSize.Height;
		if (bits == 32)
			present.BackBufferFormat = D3DFMT_X8R8G8B8;
		else
			present.BackBufferFormat = D3DFMT_R5G6B5;
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
	}

	Adapter = D3DADAPTER_DEFAULT;
	DevType = D3DDEVTYPE_HAL;

	//antialias
	if (antialias > 0)
	{
		if(antialias > 16)
			antialias = 16;

		DWORD qualityLevels = 0;

		while(antialias > 0)
		{
			if(SUCCEEDED(pID3D->CheckDeviceMultiSampleType(Adapter,
				DevType, present.BackBufferFormat, !FullScreen,
				(D3DMULTISAMPLE_TYPE)antialias, &qualityLevels)))
			{
				u32 quality = min_((u32)(qualityLevels-1), 1u);
				if (VendorID == 0x10DE)  //nvidia
				{
					switch(csaa)
					{
					case ECSAA_8x:	
						if(antialias == D3DMULTISAMPLE_4_SAMPLES && qualityLevels > 2) 
						{	
							quality = 2;
							CSAAApplied = true;
						}
						break;
					case ECSAA_8xQ:		
						if(antialias == D3DMULTISAMPLE_8_SAMPLES && qualityLevels > 0) 
						{	
							quality = 0;
							CSAAApplied = true;
						}
						break;
					case ECSAA_16x:	
						if(antialias == D3DMULTISAMPLE_4_SAMPLES && qualityLevels > 4) 
						{	
							quality = 4;
							CSAAApplied = true;
						}
						break;
					case ECSAA_16xQ:	
						if(antialias == D3DMULTISAMPLE_8_SAMPLES && qualityLevels > 2) 
						{	
							quality = 2;
							CSAAApplied = true;
						}
						break;
					}
				}

				present.MultiSampleType	= (D3DMULTISAMPLE_TYPE)antialias;
				present.MultiSampleQuality = quality;
				break;
			}
			--antialias;
		}
	}
	AntiAliasing = antialias;
	Quality = Quality;

	//depth stencil
	present.AutoDepthStencilFormat = D3DFMT_D24S8;
	if(FAILED(pID3D->CheckDeviceFormat(Adapter, DevType,
		present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, present.AutoDepthStencilFormat)))
	{
		present.AutoDepthStencilFormat = D3DFMT_D24X4S4;
		if(FAILED(pID3D->CheckDeviceFormat(Adapter, DevType,
			present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, present.AutoDepthStencilFormat)))
		{
			present.AutoDepthStencilFormat = D3DFMT_D15S1;
			if(FAILED(pID3D->CheckDeviceFormat(Adapter, DevType,
				present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, present.AutoDepthStencilFormat)))
			{
				_ASSERT(false);
			}
		}
	}
	else
		if(FAILED(pID3D->CheckDepthStencilMatch(Adapter, DevType,
			present.BackBufferFormat, present.BackBufferFormat, present.AutoDepthStencilFormat)))
		{
			_ASSERT(false);
		}

	present.EnableAutoDepthStencil	= TRUE;

	//create device
	DWORD creationFlag = multithread ? D3DCREATE_MULTITHREADED : 0;

	hr = pID3D->CreateDevice(Adapter, DevType, hwnd,
		creationFlag | D3DCREATE_HARDWARE_VERTEXPROCESSING , &present, &pID3DDevice);

	_ASSERT(SUCCEEDED(hr));				//强制hardware processing
	
	if (!pID3DDevice) { _ASSERT(false); return false; }

	//get caps
	pID3DDevice->GetDeviceCaps(&Caps);
	if (Caps.MaxVertexShaderConst < 256 ||
		!queryFeature(EVDF_STREAM_OFFSET) ||
		/*!queryFeature(EVDF_W_BUFFER) ||*/
		!queryFeature(EVDF_RENDER_TO_TARGET) ||
		!queryFeature(EVDF_MIP_MAP) ||
		!queryFeature(EVDF_TEXTURE_ADDRESS) ||
		!queryFeature(EVDF_SEPARATE_UVWRAP))
	{
		MessageBoxA(NULL, "显卡不支持某些功能，设备不能正常运行!", "警告", MB_ICONEXCLAMATION);
		return false;
	}

	MaxTextureUnits = min_((u32)Caps.MaxSimultaneousTextures, (u32)MATERIAL_MAX_TEXTURES);
	MaxUserClipPlanes = (u32)Caps.MaxUserClipPlanes;
	switch(VendorID)
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
	pID3DDevice->GetDepthStencilSurface(&DepthStencilSurface);
	D3DSURFACE_DESC desc;
	DepthStencilSurface->GetDesc(&desc);
	DepthStencilSize.set( desc.Width, desc.Height );
	
	//back buffer format
	BackBufferFormat = D3DFMT_A8R8G8B8;
	//if (SUCCEEDED(pID3DDevice->GetRenderTarget(0, &BackBuffer)))
	if (SUCCEEDED(pID3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &BackBuffer)))
	{
		D3DSURFACE_DESC desc;
		BackBuffer->GetDesc(&desc);
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
	MaterialRenderServices = new CD3D9MaterialRenderServices(queryFeature(EVDF_PIXEL_SHADER_1_1));

	createVertexDecl();	
	
	setDisplayMode(windowSize);

	resetSceneStateCache();

	setAmbientLight(SColor(255,255,255));
	setFog(SFogParam());
	deleteAllDynamicLights();

	return true;
}

void CD3D9Driver::createVertexDecl()
{
	//vertex declation
	pID3DDevice->CreateVertexDeclaration(declBasicColor, &VertexDeclarations[EVT_BASICCOLOR]);
	pID3DDevice->CreateVertexDeclaration(declBasicTex, &VertexDeclarations[EVT_BASICTEX]);
	pID3DDevice->CreateVertexDeclaration(declStandard, &VertexDeclarations[EVT_STANDARD]);
	pID3DDevice->CreateVertexDeclaration(declVertexModel, &VertexDeclarations[EVT_MODEL]);

}

void CD3D9Driver::releaseVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		SAFE_RELEASE(VertexDeclarations[i]);
	}
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
	{
		FPSCounter.registerFrame( GetTickCount() );
		return true;
	}

	if( hr == D3DERR_DEVICELOST )
	{
		DeviceLost = true;
	}

	return false;
}

bool CD3D9Driver::clear( bool renderTarget, bool zBuffer, SColor color, float z /*= 1*/ )
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
	{
		flags |= D3DCLEAR_ZBUFFER;
		flags |= D3DCLEAR_STENCIL;
	}

	if (flags)
	{
		hr = pID3DDevice->Clear( 0, NULL, flags, color.color, 1.0, 0);
		if (FAILED(hr)) { _ASSERT(false); return false; }
	}
	return true;
}

bool CD3D9Driver::queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
{
	switch (feature)
	{
	case EVDF_TEXTURE_ADDRESS:
		return (Caps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP) &&
			(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR) &&
			(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER) &&
			(Caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE);
	case EVDF_SEPARATE_UVWRAP:
		return (Caps.TextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV) != 0;
	case EVDF_RENDER_TO_TARGET:
		return Caps.NumSimultaneousRTs > 0;
	case EVDF_HARDWARE_TL:
		return (Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
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
	case EVDF_VERTEX_SHADER_4_0:
		return Caps.VertexShaderVersion >= D3DPS_VERSION(4,0);
	case EVDF_VERTEX_SHADER_5_0:
		return Caps.VertexShaderVersion >= D3DPS_VERSION(5,0);
	case EVDF_PIXEL_SHADER_1_1:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(1,1);
	case EVDF_PIXEL_SHADER_2_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(2,0);
	case EVDF_PIXEL_SHADER_3_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(3,0);
	case EVDF_PIXEL_SHADER_4_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(4,0);
	case EVDF_PIXEL_SHADER_5_0:
		return Caps.PixelShaderVersion >= D3DPS_VERSION(5,0);
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

void CD3D9Driver::deleteAllDynamicLights()
{
	for (u32 i=0; i<SceneState.LightCount; ++i)
	{
		if (ResetRenderStates || SceneState.LightsOn[i] != false)
			pID3DDevice->LightEnable(i, false);
	}

	SceneState.resetLights();
}

bool CD3D9Driver::addDynamicLight( const SLight& dl )
{
	if(SceneState.LightCount >= Caps.MaxActiveLights)
		return false;

	D3DLIGHT9 light;

	switch (dl.Type)
	{
	case ELT_POINT:
		light.Type = D3DLIGHT_POINT;
		break;
	case ELT_SPOT:
		light.Type = D3DLIGHT_SPOT;
		break;
	case ELT_DIRECTIONAL:
		light.Type = D3DLIGHT_DIRECTIONAL;
		break;
	}

	light.Position = *(D3DVECTOR*)((void*)(&dl.Position));
	light.Direction = *(D3DVECTOR*)((void*)(&dl.Direction));

	light.Range = min_(dl.Radius, MAX_DLIGHT_RADIUS);
	light.Falloff = dl.Falloff;

	light.Diffuse = *(D3DCOLORVALUE*)((void*)(&dl.DiffuseColor));
	light.Specular = *(D3DCOLORVALUE*)((void*)(&dl.SpecularColor));
	light.Ambient = *(D3DCOLORVALUE*)((void*)(&dl.AmbientColor));

	light.Attenuation0 = dl.Attenuation.X;
	light.Attenuation1 = dl.Attenuation.Y;
	light.Attenuation2 = dl.Attenuation.Z;

	light.Theta = dl.InnerCone * 2.0f * DEGTORAD;
	light.Phi = dl.OuterCone * 2.0f * DEGTORAD;

	if (D3D_OK == pID3DDevice->SetLight(SceneState.LightCount, &light))
	{
		SceneState.Lights[SceneState.LightCount] = dl;
		++SceneState.LightCount;
		return true;
	}
	return false;
}

void CD3D9Driver::turnLightOn( u32 lightIndex, bool turnOn )
{
	if(lightIndex >= SceneState.LightCount)
		return;
	
	if (ResetRenderStates || SceneState.LightsOn[lightIndex] != turnOn)
	{
		pID3DDevice->LightEnable(lightIndex, turnOn ? TRUE : FALSE);

		SceneState.LightsOn[lightIndex] = turnOn;
	}
}

void CD3D9Driver::setAmbientLight( SColor color )
{
	DEVICE_SET_RENDER_STATE(Ambient, D3DRS_AMBIENT, color.color);
	SceneState.AmbientLightColor = color;
}

void CD3D9Driver::setFog( SFogParam fogParam )
{
	DEVICE_SET_RENDER_STATE(FogColor, D3DRS_FOGCOLOR, fogParam.FogColor.color);

	DWORD fogmode = (fogParam.FogType == EFT_FOG_LINEAR)? D3DFOG_LINEAR : (fogParam.FogType == EFT_FOG_EXP)?D3DFOG_EXP : D3DFOG_EXP2;

	if (fogParam.PixelFog)
	{
		DEVICE_SET_RENDER_STATE(FogTableMode, D3DRS_FOGTABLEMODE, fogmode);
	}
	else
	{
		DEVICE_SET_RENDER_STATE(FogVertexMode, D3DRS_FOGVERTEXMODE, fogmode);
	}

	if (fogParam.FogType==EFT_FOG_LINEAR)
	{
		DEVICE_SET_RENDER_STATE(FogStart, D3DRS_FOGSTART, *(DWORD*)(&fogParam.FogStart));

		DEVICE_SET_RENDER_STATE(FogEnd, D3DRS_FOGEND, *(DWORD*)(&fogParam.FogEnd));
	}
	else
	{
		DEVICE_SET_RENDER_STATE(FogDensity, D3DRS_FOGDENSITY, *(DWORD*)(&fogParam.FogDensity));
	}

	DEVICE_SET_RENDER_STATE(RangeFogEnable, D3DRS_RANGEFOGENABLE, (fogParam.RangeFog ? TRUE : FALSE));

	SceneState.FogParam = fogParam;
}


bool CD3D9Driver::setClipPlane( u32 index, const plane3df& plane )
{
	if (index >= MaxUserClipPlanes)
		return false;

	if (plane != CurrentDeviceState.clipplanes[index])
	{
		pID3DDevice->SetClipPlane(index, (const float*)&plane);
		CurrentDeviceState.clipplanes[index] = plane;
	}
	return true;
}

void CD3D9Driver::enableClipPlane( u32 index, bool enable )
{
	if (index >= MaxUserClipPlanes)
		return;
	DWORD renderstate;
	pID3DDevice->GetRenderState(D3DRS_CLIPPLANEENABLE, &renderstate);
	if (enable)
		renderstate |= (1 << index);
	else
		renderstate &= ~(1 << index);

	if (CurrentDeviceState.enableclips[index] != renderstate)
	{
		pID3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, renderstate);
		CurrentDeviceState.enableclips[index] = renderstate;
	}	
}

bool CD3D9Driver::setRenderTarget( ITexture* texture )
{
	if (texture && !texture->isRenderTarget())
	{
		_ASSERT(false);
		return false;
	}

	CD3D9Texture* tex = static_cast<CD3D9Texture*>(texture);

	if ( tex == NULL )
	{
		if (PrevRenderTarget)		// restore backbuffer and depth/stencil
		{
			if (FAILED(pID3DDevice->SetRenderTarget(0, PrevRenderTarget)))
			{
				_ASSERT(false);
				return false;
			}
			if (FAILED(pID3DDevice->SetDepthStencilSurface(DepthStencilSurface)))
			{
				_ASSERT(false);
				return false;
			}
			CurrentRenderTargetSize.set(0,0);
			PrevRenderTarget->Release();
			PrevRenderTarget = NULL;
		}
	}
	else
	{
		if (!PrevRenderTarget)			//restore old one (backbuffer)
		{
			if (FAILED(pID3DDevice->GetRenderTarget(0, &PrevRenderTarget)))
			{
				_ASSERT(false);
				return false;
			}
		}

		if (FAILED(pID3DDevice->SetRenderTarget(0, tex->getRTTSurface())))
		{
			_ASSERT(false);
			return false;
		}

		CurrentRenderTargetSize = tex->getSize();

		if (FAILED(pID3DDevice->SetDepthStencilSurface(tex->getRTTDepthSurface())))
		{
			_ASSERT(false);
			return false;
		}
	}
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

void CD3D9Driver::registerLostReset( ILostResetCallback* callback )
{
	LostResetList.push_back(callback);
}

void CD3D9Driver::setDisplayMode( const dimension2du& size )
{
	ScreenSize = size;
	
	dimension2du vpsize = ScreenSize;

	setViewPort(recti(0,0,vpsize.Width, vpsize.Height));

	present.BackBufferWidth = vpsize.Width;
	present.BackBufferHeight = vpsize.Height;
	
	reset();
}

bool CD3D9Driver::changeDriverSettings( bool vsync, u32 antialias )
{
	bool change = false;
	if (vsync != VSync)
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
			if(SUCCEEDED(pID3D->CheckDeviceMultiSampleType(Adapter,
				DevType, present.BackBufferFormat, !FullScreen,
				(D3DMULTISAMPLE_TYPE)antialias, &qualityLevels)))
			{
				break;
			}
			--antialias;
		}
	}

	if (antialias != AntiAliasing)
	{
		present.MultiSampleType = (D3DMULTISAMPLE_TYPE)antialias;
		change = true;
	}

	bool ret = true;
	if (change)
	{
		if (beginScene())
		{
			clear(true, true, SColor(0,0,0));
			endScene();
		}

		ret = reset();

		if(ret)
		{ 
			AntiAliasing = antialias;
			VSync = vsync;
		}
	}
	return ret;
}

void CD3D9Driver::draw3DMode(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType,
	u32 primCount, SDrawParam drawParam)
{
	setRenderState3DMode();

	//draw
	CD3D9Effect* effect = static_cast<CD3D9Effect*>(Material.Effect);

	ID3DXEffect* pEffect = NULL; 
	u32 cPasses = 0; 

	if (effect)
	{
		pEffect = effect->getEffect();
		cPasses = effect->NumPasses;

		ShaderServices->setShaderConstants(effect, Material);
	}
	else		
	{
		cPasses = MaterialRenderer->getNumPasses();

		ShaderServices->setShaderConstants(Material.VertexShader, Material);
	}

	if (pEffect)
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			pEffect->BeginPass(iPass);

			pEffect->CommitChanges();

			drawIndexedPrimitive(vbuffer, ibuffer, vbuffer2, primType,
				primCount, drawParam);

			pEffect->EndPass();
		}
	}
	else
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			drawIndexedPrimitive(vbuffer, ibuffer, vbuffer2, primType,
				primCount, drawParam);
		}
	}

}

void CD3D9Driver::draw3DMode( IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 startIndex )
{
	setRenderState3DMode();

	//draw
	CD3D9Effect* effect = static_cast<CD3D9Effect*>(Material.Effect);

	ID3DXEffect* pEffect = NULL; 
	u32 cPasses = 0; 

	if (effect)
	{
		pEffect = effect->getEffect();
		cPasses = effect->NumPasses;

		ShaderServices->setShaderConstants(effect, Material);
	}
	else		
	{
		cPasses = MaterialRenderer->getNumPasses();

		ShaderServices->setShaderConstants(Material.VertexShader, Material);
	}

	if (pEffect)
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			pEffect->BeginPass(iPass);

			pEffect->CommitChanges();

			drawPrimitive(vbuffer, primType, primCount, voffset, startIndex);

			pEffect->EndPass();
		}
	}
	else
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			drawPrimitive(vbuffer, primType, primCount, voffset, startIndex);
		}
	}
}

void CD3D9Driver::draw3DModeUP( void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType )
{
	setRenderState3DMode();

	//draw
	CD3D9Effect* effect = static_cast<CD3D9Effect*>(Material.Effect);

	ID3DXEffect* pEffect = NULL; 
	u32 cPasses = 0; 

	if (effect)
	{
		pEffect = effect->getEffect();
		cPasses = effect->NumPasses;

		ShaderServices->setShaderConstants(effect, Material);
	}
	else		
	{
		cPasses = MaterialRenderer->getNumPasses();

		ShaderServices->setShaderConstants(Material.VertexShader, Material);
	}

	if (pEffect)
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			pEffect->BeginPass(iPass);

			pEffect->CommitChanges();

			drawIndexedPrimitiveUP(vertices, vType, indices, iType, iCount, vStart, vCount, primType);

			pEffect->EndPass();
		}
	}
	else
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			drawIndexedPrimitiveUP(vertices, vType, indices, iType, iCount, vStart, vCount, primType);
		}
	}
}

void CD3D9Driver::draw3DModeUP( void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType )
{
	setRenderState3DMode();

	//draw
	CD3D9Effect* effect = static_cast<CD3D9Effect*>(Material.Effect);

	ID3DXEffect* pEffect = NULL; 
	u32 cPasses = 0; 

	if (effect)
	{
		pEffect = effect->getEffect();
		cPasses = effect->NumPasses;

		ShaderServices->setShaderConstants(effect, Material);
	}
	else		
	{
		cPasses = MaterialRenderer->getNumPasses();

		ShaderServices->setShaderConstants(Material.VertexShader, Material);
	}

	if (pEffect)
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			pEffect->BeginPass(iPass);

			pEffect->CommitChanges();

			drawPrimitiveUP(vertices, vType, vCount, primType);

			pEffect->EndPass();
		}
	}
	else
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			drawPrimitiveUP(vertices, vType, vCount, primType);
		}
	}
}

void CD3D9Driver::draw2DMode( IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType,
	u32 primCount, SDrawParam drawParam,
	bool alpha, bool texture, bool alphaChannel )
{
	setRenderState2DMode(alpha, texture, alphaChannel);

	drawIndexedPrimitive(vbuffer, ibuffer, NULL, primType,
		primCount, drawParam);
}

void CD3D9Driver::draw2DMode( IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primTpye, u32 primCount, u32 vOffset, u32 vStart,
	bool alpha, bool texture, bool alphaChannel )
{
	setRenderState2DMode(alpha, texture, alphaChannel);

	drawPrimitive(vbuffer, primTpye, primCount, vOffset, vStart);
}

void CD3D9Driver::draw2DModeUP( void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel )
{
	setRenderState2DMode(alpha, texture, alphaChannel);

	drawIndexedPrimitiveUP(vertices, vType, indices, iType, iCount, vStart, vCount, primType);
}

void CD3D9Driver::draw2DModeUP( void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel )
{
	setRenderState2DMode(alpha, texture, alphaChannel);

	drawPrimitiveUP(vertices, vType, vCount, primType);
}

void CD3D9Driver::drawIndexedPrimitive( IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType, 
	u32 primCount, SDrawParam drawParam)
{
	HRESULT hr;

	setVertexDeclaration(vbuffer->Type);

	u32 stride = getVertexPitchFromType(vbuffer->Type);
	D3DFORMAT indexType = CD3D9Helper::getIndexType(ibuffer->Type);

	_ASSERT( primCount < 20000 );
	if (!drawParam.numVertices || !primCount)
	{
		_ASSERT(false);
		return;
	}

	//stream 0
	if (CurrentDeviceState.vBuffer != vbuffer ||
		CurrentDeviceState.vOffset != drawParam.voffset)
	{
		pID3DDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)vbuffer->HWLink, drawParam.voffset * stride, stride);

		CurrentDeviceState.vBuffer = vbuffer;
		CurrentDeviceState.vOffset = drawParam.voffset;
	}
	
	//stream 1
	
	if (vbuffer2)
	{
		u32 stride2 = getVertexPitchFromType(vbuffer2->Type);

		if (CurrentDeviceState.vBuffer2 != vbuffer2 ||
			CurrentDeviceState.vOffset2 != drawParam.voffset2)
		{
			pID3DDevice->SetStreamSource(1, (IDirect3DVertexBuffer9*)vbuffer2->HWLink, drawParam.voffset2 * stride2, stride2);

			CurrentDeviceState.vBuffer2 = vbuffer2;
			CurrentDeviceState.vOffset2 = drawParam.voffset2;
		}
	}

	if (CurrentDeviceState.iBuffer != ibuffer)
	{
		pID3DDevice->SetIndices((IDirect3DIndexBuffer9*)ibuffer->HWLink);
		CurrentDeviceState.iBuffer = ibuffer;
	}
	
	switch(primType)
	{
	case EPT_POINTS:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_POINTLIST, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		break;
	case EPT_LINE_STRIP:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_LINESTRIP, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		break;
	case EPT_LINES:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		break;
	case EPT_TRIANGLE_STRIP:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		PrimitivesDrawn += primCount;
		break;
	case EPT_TRIANGLE_FAN:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		PrimitivesDrawn += primCount;
		break;
	case EPT_TRIANGLES:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		PrimitivesDrawn += primCount;
		break;
	}

	if (FAILED(hr))
		_ASSERT(false);

	++DrawCall;
}

void CD3D9Driver::drawIndexedPrimitiveUP( void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType )
{
	HRESULT hr;

	setVertexDeclaration(vType);
	u32 stride = getVertexPitchFromType(vType);
	D3DFORMAT indexType = CD3D9Helper::getIndexType(iType);
	u32 primitiveCount = ::getPrimitiveCount(primType, iCount);

	_ASSERT( primitiveCount < 20000 );
	if (!vCount || !primitiveCount)
	{
		return;
	}

	switch(primType)
	{
	case EPT_POINTS:
		hr = pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_POINTLIST, vStart, 
			vCount, primitiveCount, indices, indexType, vertices, stride);
		break;
	case EPT_LINE_STRIP:
		hr = pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINESTRIP, vStart, 
			vCount, primitiveCount, indices, indexType, vertices, stride);
		break;
	case EPT_LINES:
		hr = pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, vStart, 
			vCount, primitiveCount, indices, indexType, vertices, stride);
		break;
	case EPT_TRIANGLE_STRIP:
		hr = pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, vStart, 
			vCount, primitiveCount, indices, indexType, vertices, stride);
		PrimitivesDrawn += primitiveCount;
		break;
	case EPT_TRIANGLE_FAN:
		hr = pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLEFAN, vStart, 
			vCount, primitiveCount, indices, indexType, vertices, stride);
		PrimitivesDrawn += primitiveCount;
		break;
	case EPT_TRIANGLES:
		hr = pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, vStart, 
			vCount, primitiveCount, indices, indexType, vertices, stride);
		PrimitivesDrawn += primitiveCount;
		break;
	}

	if (FAILED(hr))
		_ASSERT(false);

	//clear state
	CurrentDeviceState.vBuffer = NULL;
	CurrentDeviceState.iBuffer = NULL;
	CurrentDeviceState.vOffset = 0;

	++DrawCall;
}

void CD3D9Driver::drawPrimitive( IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 vStart )
{
	HRESULT hr;

	setVertexDeclaration(vbuffer->Type);
	u32 stride = getVertexPitchFromType(vbuffer->Type);

	_ASSERT( primCount < 20000 );

	//stream 0
	if (CurrentDeviceState.vBuffer != vbuffer ||
		CurrentDeviceState.vOffset != voffset)
	{
		pID3DDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)vbuffer->HWLink, voffset * stride, stride);

		CurrentDeviceState.vBuffer = vbuffer;
		CurrentDeviceState.vOffset = voffset;
	}

	switch(primType)
	{
	case EPT_POINTS:
		hr = pID3DDevice->DrawPrimitive(D3DPT_POINTLIST, vStart, primCount);
		break;
	case EPT_LINE_STRIP:
		hr = pID3DDevice->DrawPrimitive(D3DPT_LINESTRIP, vStart, primCount);
		break;
	case EPT_LINES:
		hr = pID3DDevice->DrawPrimitive(D3DPT_LINELIST, vStart, primCount);
		break;
	case EPT_TRIANGLE_STRIP:
		hr = pID3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, vStart, primCount);
		PrimitivesDrawn += primCount;
		break;
	case EPT_TRIANGLE_FAN:
		hr = pID3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, vStart, primCount);
		PrimitivesDrawn += primCount;
		break;
	case EPT_TRIANGLES:
		hr = pID3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, vStart, primCount);
		PrimitivesDrawn += primCount;
		break;
	}

	if (FAILED(hr))
		_ASSERT(false);

	++DrawCall;
}

void CD3D9Driver::drawPrimitiveUP( void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType )
{
	HRESULT hr;

	setVertexDeclaration(vType);
	u32 stride = getVertexPitchFromType(vType);
	u32 primitiveCount = ::getPrimitiveCount(primType, vCount);

	_ASSERT( primitiveCount < 20000 );

	switch(primType)
	{
	case EPT_POINTS:
		hr = pID3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST, primitiveCount, vertices, stride);
		break;
	case EPT_LINE_STRIP:
		hr = pID3DDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, primitiveCount, vertices, stride);
		break;
	case EPT_LINES:
		hr = pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, primitiveCount, vertices, stride);
		break;
	case EPT_TRIANGLE_STRIP:
		hr = pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, primitiveCount, vertices, stride);
		PrimitivesDrawn += primitiveCount;
		break;
	case EPT_TRIANGLE_FAN:
		hr = pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, primitiveCount, vertices, stride);
		PrimitivesDrawn += primitiveCount;
		break;
	case EPT_TRIANGLES:
		hr = pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, primitiveCount, vertices, stride);
		PrimitivesDrawn += primitiveCount;
		break;
	}

	if (FAILED(hr))
		_ASSERT(false);

	//clear state
	CurrentDeviceState.vBuffer = NULL;
	CurrentDeviceState.vOffset = 0;

	++DrawCall;
}

bool CD3D9Driver::reset()
{
	for( T_LostResetList::iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onLost();

	//depth stencil buffer
	SAFE_RELEASE(DepthStencilSurface);
	DepthStencilSize.set(0,0);

	//back buffer
	SAFE_RELEASE(BackBuffer);

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
	if (SUCCEEDED(pID3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &BackBuffer)))
	//if (SUCCEEDED(pID3DDevice->GetRenderTarget(0, &BackBuffer)))
	{
		D3DSURFACE_DESC desc;
		BackBuffer->GetDesc(&desc);
		BackBufferFormat = desc.Format;

		if (BackBufferFormat == D3DFMT_X8R8G8B8)
			BackBufferFormat = D3DFMT_A8R8G8B8;
	}

	//depth buffer size
	pID3DDevice->GetDepthStencilSurface(&DepthStencilSurface);
	D3DSURFACE_DESC desc;
	DepthStencilSurface->GetDesc(&desc);
	DepthStencilSize.set( desc.Width, desc.Height );
		
	for( T_LostResetList::iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onReset();

	DeviceLost = false;

	//reset
	LastMaterial.MaterialType = (E_MATERIAL_TYPE)-1;
	ResetRenderStates = true;
	CurrentRenderMode = ERM_NONE;
	CurrentDeviceState.reset();
	resetSceneStateCache();

	//restore scene states
	setAmbientLight(SceneState.AmbientLightColor);
	u32 lightCount =  SceneState.LightCount;
	SceneState.LightCount = 0;
	for (u32 i=0; i<lightCount; ++i)
	{
		addDynamicLight(SceneState.Lights[i]);
		turnLightOn(i, SceneState.LightsOn[i]);
	}
	setFog(SceneState.FogParam);

	return true;
}

void CD3D9Driver::setVertexDeclaration( E_VERTEX_TYPE type )
{
	if (CurrentDeviceState.VertexType != type)
	{
		IDirect3DVertexDeclaration9* decl = getD3DVertexDeclaration(type);
		pID3DDevice->SetVertexDeclaration(decl);

		CurrentDeviceState.VertexType = type;
	}
}

void CD3D9Driver::setRenderState3DMode()
{
	if ( CurrentRenderMode != ERM_3D )
	{
		CurrentRenderMode = ERM_3D;
	}

	if (ResetRenderStates || LastMaterial != Material )
	{
		if (LastMaterial.MaterialType != Material.MaterialType)
		{
			IMaterialRenderer* renderer = MaterialRenderServices->getMaterialRenderer(LastMaterial.MaterialType);
			if (renderer)
				renderer->OnUnsetMaterial();
		}

		MaterialRenderServices->setBasicRenderStates( Material, LastMaterial, ResetRenderStates );

		MaterialRenderer = MaterialRenderServices->getMaterialRenderer(Material.MaterialType);

		// set new material.
		MaterialRenderer->OnSetMaterial( Material, LastMaterial, ResetRenderStates);	
	}

	LastMaterial = Material;
	ResetRenderStates = false;
}

void CD3D9Driver::setRenderState2DMode( bool alpha, bool texture, bool alphaChannel )
{
	if ( CurrentRenderMode != ERM_2D )
	{
		matrix4 matWorld(true);
		//pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)(void*)matWorld.pointer());
		setTransform(ETS_WORLD, matWorld);

		matrix4 matView(true);
		matView.setTranslation(vector3df(-0.5f,-0.5f,0));
		//pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)(void*)matView.pointer());
		setTransform(ETS_VIEW, matView);

		matrix4 matProject(true);
		const dimension2du& renderTargetSize = ScreenSize;
		matProject.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
		matProject.setTranslation(vector3df(-1,1,0));
		//pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)(void*)matProject.pointer());
		setTransform(ETS_PROJECTION, matProject);

		CurrentRenderMode = ERM_2D;
	}

	if (ResetRenderStates || LastMaterial != InitMaterial2D )
	{
		MaterialRenderServices->setBasicRenderStates(InitMaterial2D, LastMaterial, ResetRenderStates);
	}

	MaterialRenderServices->set2DRenderStates(alpha, texture, alphaChannel, ResetRenderStates);

	LastMaterial = InitMaterial2D;

	ResetRenderStates = false;
}

void CD3D9Driver::drawDebugInfo( const c8* strMsg )
{
	if (DeviceLost)
		return;

	vector2di pos = vector2di(5,5);

// 	sprintf_s(DebugMsg, 512, "显卡: %s\n分辨率: %d X %d\nFPS: %0.1f\n三角形数: %d\n三角形Draw次数: %d\n小区域内存剩余(%%): %0.1f\n区域内存剩余(%%): %0.1f\n", 
// 		DeviceDescription, present.BackBufferWidth, present.BackBufferHeight, getFPS(), PrimitivesDrawn, DrawCall,
// 		Z_AvailableSmallMemoryPercent() * 100,
// 		Z_AvailableMemoryPercent() * 100);

 	sprintf_s(DebugMsg, 512, "Dev: %s\nRes: %d X %d\nFPS: %0.1f\nTriangles: %d\nDraw Call: %d\n", 
 		DeviceDescription, present.BackBufferWidth, present.BackBufferHeight, getFPS(), PrimitivesDrawn, DrawCall);

	strcat_s(DebugMsg, 512, strMsg);

	g_Engine->getFont()->drawA(DebugMsg, SColor(0,255,0), pos);
}

void CD3D9Driver::resetSceneStateCache()
{
	pID3DDevice->GetRenderState(D3DRS_AMBIENT, &SSCache.Ambient);
	pID3DDevice->GetRenderState(D3DRS_FOGCOLOR, &SSCache.FogColor);
	pID3DDevice->GetRenderState(D3DRS_FOGTABLEMODE, &SSCache.FogTableMode);
	pID3DDevice->GetRenderState(D3DRS_FOGVERTEXMODE, &SSCache.FogVertexMode);
	pID3DDevice->GetRenderState(D3DRS_FOGSTART, &SSCache.FogStart);
	pID3DDevice->GetRenderState(D3DRS_FOGEND, &SSCache.FogEnd);
	pID3DDevice->GetRenderState(D3DRS_FOGDENSITY, &SSCache.FogDensity);
	pID3DDevice->GetRenderState(D3DRS_RANGEFOGENABLE, &SSCache.RangeFogEnable);
}
