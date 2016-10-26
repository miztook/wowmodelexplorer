#include "stdafx.h"
#include "CD3D9Driver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Helper.h"
#include "CD3D9RenderTarget.h"
#include "CD3D9Shader.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9MaterialRenderServices.h"
#include "CD3D9SceneStateServices.h"
#include "CD3D9VertexDeclaration.h"

#define  DEVICE_SET_RENDER_STATE(prop, d3drs, v)	if (SSCache.prop != v)		\
		{	pID3DDevice->SetRenderState(d3drs, v);				\
		SSCache.prop = v;	} 

typedef IDirect3D9* (__stdcall *D3DCREATETYPE)(UINT);
typedef HRESULT (__stdcall *D3DCREATETYPEEX)(UINT, IDirect3D9Ex**);

CD3D9Driver::CD3D9Driver()
{
	HLib = NULL_PTR;
	pID3D = NULL_PTR;
	pID3DDevice = NULL_PTR;

	HWnd = NULL_PTR;
	DevType = D3DDEVTYPE_HAL;

	DeviceLost = false;

	DefaultDepthBuffer = NULL_PTR;
	DefaultBackBuffer = NULL_PTR;
	BackBufferFormat = D3DFMT_UNKNOWN;

	PrimitivesDrawn = 0;
	DrawCall = 0;
	AlphaToCoverageSupport = false;

	CurrentDeviceState.reset();
	CurrentRenderMode = ERM_NONE;
	ResetRenderStates = true;
	CurrentRenderTarget = NULL_PTR;

	MaterialRenderer = NULL_PTR;
	ShaderServices = NULL_PTR;
	MaterialRenderServices = NULL_PTR;
	SceneStateServices = NULL_PTR;

	D3D9SceneStateServices = NULL_PTR;
	D3D9ShaderServices = NULL_PTR;
	D3D9MaterialRenderServices = NULL_PTR;

	AdapterCount = 0;

	//2D
	InitMaterial2D.MaterialType = EMT_2D;
	InitMaterial2D.Cull = ECM_BACK;
	InitMaterial2D.Lighting = false;
	InitMaterial2D.ZWriteEnable = false;
	InitMaterial2D.ZBuffer = ECFN_NEVER;
	InitMaterial2D.AntiAliasing = EAAM_OFF;
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		InitMaterial2D.TextureLayer[i].TextureWrapU=ETC_CLAMP;
		InitMaterial2D.TextureLayer[i].TextureWrapV=ETC_CLAMP;
		InitMaterial2D.TextureLayer[i].UseTextureMatrix = false;

		InitOverrideMaterial2D.TextureFilters[i] = ETF_NONE;
	}
	InitMaterial2DZTest = InitMaterial2D;
	InitMaterial2DZTest.ZBuffer = ECFN_LESSEQUAL;
	InitMaterial2DScissorTest = InitMaterial2D;
	InitMaterial2DScissorTest.ScissorEnable = true;
	InitMaterial2DScissorZTest = InitMaterial2D;
	InitMaterial2DScissorZTest.ScissorEnable = true;
	InitMaterial2DScissorZTest.ZBuffer = ECFN_LESSEQUAL;

	memset(&Present, 0, sizeof(Present));
	memset(&Caps, 0, sizeof(Caps));
	memset(VertexDeclarations, 0, sizeof(VertexDeclarations));
	memset(DebugMsg, 0, sizeof(DebugMsg));
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

bool CD3D9Driver::initDriver(const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{
	ASSERT(::IsWindow(wndInfo.hwnd));

	HWnd = wndInfo.hwnd;
	DriverSetting.vsync = vsync;
	DriverSetting.fullscreen = fullscreen;

	RECT rc;
	::GetClientRect(wndInfo.hwnd, &rc);
	dimension2du windowSize((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);

	HLib = ::LoadLibraryA("d3d9.dll");
	if (!HLib)
	{
		CSysUtility::messageBoxWarning("can't load d3d9.dll，Program Exits!");
		ASSERT(false);
		return false;
	}

	HRESULT hr;

	if (!g_Engine->getOSInfo()->IsAeroSupport())
	{
		D3DCREATETYPE d3dcreate = (D3DCREATETYPE)::GetProcAddress(HLib, "Direct3DCreate9");
		if(!d3dcreate) { ASSERT(false); return false; }
		pID3D = (d3dcreate)(D3D_SDK_VERSION);
		if (!pID3D) { ASSERT(false); return false; }

		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX9");
	}
	else
	{
		IDirect3D9Ex* pID3DEx = NULL_PTR;
		D3DCREATETYPEEX d3dcreate = (D3DCREATETYPEEX)::GetProcAddress(HLib, "Direct3DCreate9Ex");
		if(!d3dcreate) { ASSERT(false); return false; }
		hr = (d3dcreate)(D3D_SDK_VERSION, &pID3DEx);
		if(FAILED(hr)) { ASSERT(false); return false; }
		pID3D = pID3DEx;

		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX9Ex");
	}
	
	//device info
	AdapterCount = pID3D->GetAdapterCount();
	AdapterInfo.index = (adapter >= AdapterCount ? 0 : adapter);

	D3DADAPTER_IDENTIFIER9 identifier;
	if (FAILED(pID3D->GetAdapterIdentifier(adapter, 0, &identifier)))
	{
		AdapterInfo.index = D3DADAPTER_DEFAULT;
		if(FAILED(pID3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &identifier )))
		{
			ASSERT(false);
			return false;
		}
	}
	
	Q_strcpy( AdapterInfo.description, 512,  identifier.Description );
	Q_strcpy( AdapterInfo.name, DEFAULT_SIZE,  identifier.DeviceName );

	AdapterInfo.vendorID = identifier.VendorId;
	switch (identifier.VendorId)
	{
	case 0x1002: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "ATI Technologies Inc."); break;
	case 0x10DE: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "NVIDIA Corporation"); break;
	case 0x102B: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "Matrox Electronic System Ltd."); break;
	case 0x121A: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "3dfx Interactive Inc"); break;
	case 0x5333: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "S3 Graphics Co. Ltd"); break;
	case 0x8086: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "Intel Corporation"); break;
	default:
		Q_sprintf(AdapterInfo.vendorName, DEFAULT_SIZE, "Unknown VendorId: %x", identifier.VendorId);
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
		AdapterInfo.displayModes.emplace_back(displayMode);
	}

	//log
	g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Adapter Name: %s", AdapterInfo.name);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Adpater Description: %s", AdapterInfo.description);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Adapter Vendor: %s", AdapterInfo.vendorName);

	ZeroMemory(&Present, sizeof(Present));

	Present.BackBufferCount	= 1;
	Present.Flags |= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	if (vsync)
		Present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	else
		Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (DriverSetting.fullscreen)
	{
		SDisplayMode dm = AdapterInfo.getCloseMatchDisplayMode(windowSize.Width, windowSize.Height);

		Present.BackBufferWidth = dm.width;
		Present.BackBufferHeight = dm.height;
		Present.BackBufferFormat = d3ddm.Format;
		Present.SwapEffect	= D3DSWAPEFFECT_FLIP;
		Present.Windowed	= FALSE;
		Present.FullScreen_RefreshRateInHz = dm.refreshrate;

		//adjust
		windowSize.Width = dm.width;
		windowSize.Height = dm.height;
	}
	else
	{
		Present.BackBufferWidth = windowSize.Width;
		Present.BackBufferHeight = windowSize.Height;
		Present.BackBufferFormat = d3ddm.Format;
		Present.SwapEffect		= D3DSWAPEFFECT_DISCARD;
		Present.Windowed		= TRUE;
		Present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
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
				DevType, Present.BackBufferFormat, !fullscreen,
				(D3DMULTISAMPLE_TYPE)antialias, &qualityLevels)) && qualityLevels)
			{
				u32 quality = min_((u32)(qualityLevels-1), 1u);
				Present.MultiSampleType	= (D3DMULTISAMPLE_TYPE)antialias;
				Present.MultiSampleQuality = quality;
				Present.SwapEffect		= D3DSWAPEFFECT_DISCARD;		//multisample requies discard
				break;
			}
			--antialias;
		}
	}
	DriverSetting.antialias = antialias;
	DriverSetting.quality = (u8)Present.MultiSampleQuality;

	//depth stencil
	Present.AutoDepthStencilFormat = D3DFMT_D24X8;
	if(FAILED(pID3D->CheckDeviceFormat(AdapterInfo.index, DevType,
		Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
		D3DRTYPE_SURFACE, Present.AutoDepthStencilFormat)))
	{
		Present.AutoDepthStencilFormat = D3DFMT_D24S8;
		if(FAILED(pID3D->CheckDeviceFormat(AdapterInfo.index, DevType,
			Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, Present.AutoDepthStencilFormat)))
		{
			Present.AutoDepthStencilFormat = D3DFMT_D15S1;
			if(FAILED(pID3D->CheckDeviceFormat(AdapterInfo.index, DevType,
				Present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, Present.AutoDepthStencilFormat)))
			{
				ASSERT(false);
			}
		}
	}
	else
		if(FAILED(pID3D->CheckDepthStencilMatch(AdapterInfo.index, DevType,
			Present.BackBufferFormat, Present.BackBufferFormat, Present.AutoDepthStencilFormat)))
		{
			ASSERT(false);
		}

	Present.EnableAutoDepthStencil	= TRUE;

	//create device
	DWORD creationFlag = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
	if (multithread)
		creationFlag |= D3DCREATE_MULTITHREADED;

	if (!g_Engine->getOSInfo()->IsAeroSupport())
	{
		hr = pID3D->CreateDevice(AdapterInfo.index, DevType, wndInfo.hwnd,
			creationFlag , &Present, &pID3DDevice);

		ASSERT(SUCCEEDED(hr));				//强制hardware processing
	}
	else
	{
		IDirect3DDevice9Ex*	pID3DDeviceEx;
		IDirect3D9Ex* pID3DEx = (IDirect3D9Ex*)pID3D;
		hr = pID3DEx->CreateDeviceEx(AdapterInfo.index, DevType, wndInfo.hwnd,
			creationFlag , &Present, NULL_PTR, &pID3DDeviceEx);

		ASSERT(SUCCEEDED(hr));				//强制hardware processing

		pID3DDevice = pID3DDeviceEx;
	}

	if (!pID3DDevice) { ASSERT(false); return false; }

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Device Format: %dX%d, %s, %s", Present.BackBufferWidth, Present.BackBufferHeight, Present.Windowed ? "Window" : "FullScreen", CD3D9Helper::getD3DFormatString(Present.BackBufferFormat));
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Device Vsync: %s, MultiSample: %d, DepthStencil Format:%s", 
		Present.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE ? "Off" : "On",
		(int)Present.MultiSampleType, CD3D9Helper::getD3DFormatString(Present.AutoDepthStencilFormat));

	//get caps
	pID3DDevice->GetDeviceCaps(&Caps);

	//log
	logCaps();

	if (Caps.MaxVertexShaderConst < 256 ||
		!queryFeature(EVDF_PIXEL_SHADER_2_0) ||		//ps_1_1 not allowed
		!queryFeature(EVDF_STREAM_OFFSET) ||
		!queryFeature(EVDF_RENDER_TO_TARGET) ||
		!queryFeature(EVDF_MIP_MAP) ||
		!queryFeature(EVDF_TEXTURE_ADDRESS) ||
		!queryFeature(EVDF_SEPARATE_UVWRAP))
	{
		CSysUtility::messageBoxWarning("Video Card does not support some features. Application will now exit!");
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
		ASSERT(false);
		return false;
	}

	//shader, material
	ShaderServices = 
		D3D9ShaderServices = new CD3D9ShaderServices;

	MaterialRenderServices = 
		D3D9MaterialRenderServices = new CD3D9MaterialRenderServices;

	SceneStateServices = 
		D3D9SceneStateServices = new CD3D9SceneStateServices;

	createVertexDecl();	
	
	setDisplayMode(windowSize);

	return true;
}

void CD3D9Driver::logCaps()
{
		IFileSystem* fs = g_Engine->getFileSystem();

		fs->writeLog(ELOG_GX, "Device Caps:");
		fs->writeLog(ELOG_GX, "\tMaxVertexShaderConst: %d", Caps.MaxVertexShaderConst);
		fs->writeLog(ELOG_GX, "\tMaxStreams: %d", Caps.MaxStreams);
		fs->writeLog(ELOG_GX, "\tMaxStreamStride: %d", Caps.MaxStreamStride);
		fs->writeLog(ELOG_GX, "\tMaxSimultaneousRTs: %d", Caps.NumSimultaneousRTs);
		if (queryFeature(EVDF_PIXEL_SHADER_3_0))
			fs->writeLog(ELOG_GX, "\tPixel Shader: 3.0");
		else if (queryFeature(EVDF_PIXEL_SHADER_2_0))
			fs->writeLog(ELOG_GX, "\tPixel Shader: 2.0");
		else
			fs->writeLog(ELOG_GX, "\tPixel Shader 2.0 minimum not supported!");

		if (queryFeature(EVDF_VERTEX_SHADER_3_0))
			fs->writeLog(ELOG_GX, "\tVertex Shader: 3.0");
		else if (queryFeature(EVDF_VERTEX_SHADER_2_0))
			fs->writeLog(ELOG_GX, "\tVertex Shader: 2.0");
		else
			fs->writeLog(ELOG_GX, "\tVertex Shader 2.0 minimum not supported!");
}

bool CD3D9Driver::checkValid()
{
	HRESULT hr;

	if (DeviceLost)
	{
		if (FAILED(hr = pID3DDevice->TestCooperativeLevel()))
		{
			if (hr == D3DERR_DEVICELOST)
			{
				SLEEP(100);
				hr = pID3DDevice->TestCooperativeLevel();
				if (hr == D3DERR_DEVICELOST)
					return false;
			}
			if ( hr == D3DERR_DEVICENOTRESET &&
				!reset() )
				return false;
		}
	}

	return true;
}

bool CD3D9Driver::beginScene()
{
	HRESULT hr;

	PrimitivesDrawn = 0;
	DrawCall = 0;

	hr = pID3DDevice->BeginScene();
	if ( FAILED(hr) )
	{
		if( hr == D3DERR_DEVICELOST )
		{
			DeviceLost = true;
		}
		return false;
	}
	return true;
}

bool CD3D9Driver::endScene()
{
	pID3DDevice->EndScene();

	HRESULT hr;
	if (g_Engine->getOSInfo()->IsAeroSupport())
		hr = ((IDirect3DDevice9Ex*)pID3DDevice)->PresentEx(NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, 0);
	else
		hr = pID3DDevice->Present(NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

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
				SLEEP(100);
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
		hr = pID3DDevice->Clear( 0, NULL_PTR, flags, color.color, 1.0f, 0);
		if (FAILED(hr)) { ASSERT(false); return false; }
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
	case EVDF_VERTEX_SHADER_2_0:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(2,0);
	case EVDF_VERTEX_SHADER_3_0:
		return Caps.VertexShaderVersion >= D3DVS_VERSION(3,0);
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
	case EVDF_TEXTURE_MULTISAMPLING:
		return AlphaToCoverageSupport;
	default:
		return false;
	};
}

void CD3D9Driver::setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat )
{
	ASSERT( 0 <= state && state < ETS_COUNT );
	switch( state )
	{
	case ETS_VIEW:
		{
#ifdef FIXPIPELINE
			pID3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)(void*)mat.pointer());
#endif
			Matrices[ETS_VIEW] = mat;

			WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}
		break;
	case ETS_WORLD:
		{
#ifdef FIXPIPELINE
			pID3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(void*)mat.pointer());
#endif
			Matrices[ETS_WORLD] = mat;

			WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}
		break;
	case ETS_PROJECTION:
		{
#ifdef FIXPIPELINE
			pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)(void*)mat.pointer());
#endif
			Matrices[ETS_PROJECTION] = mat;

			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}	
		break;
	default:		//texture
		{
			s32 tex = state - ETS_TEXTURE_0;
			if (  tex >= 0  && tex < MATERIAL_MAX_TEXTURES )
			{
#ifdef FIXPIPELINE
				pID3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+ tex),
				(D3DMATRIX*)((void*)mat.pointer()));
#endif
				Matrices[state] = mat;
			}
		} 
		break;
	}
}

void CD3D9Driver::setTexture( u32 stage, ITexture* texture )
{
	D3D9MaterialRenderServices->setSampler_Texture(stage, texture);
}

void CD3D9Driver::setTransform(const matrix4& matView, const matrix4& matProjection)
{
#ifdef FIXPIPELINE
	pID3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)(void*)matView.pointer());
	pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)(void*)matProjection.pointer());
#endif
	Matrices[ETS_VIEW] = matView;
	Matrices[ETS_PROJECTION] = matProjection;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;
}

void CD3D9Driver::setTransform_Material_Textures( const matrix4& matWorld, const SMaterial& material, ITexture* const textures[], u32 numTextures )
{
#ifdef FIXPIPELINE
	pID3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(void*)matWorld.pointer());
#endif
	Matrices[ETS_WORLD] = matWorld;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;

	Material = material;

	u32 n = min_(numTextures, (u32)MATERIAL_MAX_TEXTURES);
	for (u32 i=0; i<n; ++i)
	{
		D3D9MaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

void CD3D9Driver::setTransform_Material_Textures( const matrix4& matWorld, const matrix4& matView, const matrix4& matProjection, const SMaterial& material, ITexture* const textures[], u32 numTextures )
{
#ifdef FIXPIPELINE
	pID3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(void*)matWorld.pointer());
	pID3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)(void*)matView.pointer());
	pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)(void*)matProjection.pointer());
#endif
	Matrices[ETS_WORLD] = matWorld;
	Matrices[ETS_VIEW] = matView;
	Matrices[ETS_PROJECTION] = matProjection;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;

	Material = material;

	u32 n = min_(numTextures, (u32)MATERIAL_MAX_TEXTURES);
	for (u32 i=0; i<n; ++i)
	{
		D3D9MaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

ITexture* CD3D9Driver::getTexture( u32 index ) const
{
	return D3D9MaterialRenderServices->getSampler_Texture(index);
}

bool CD3D9Driver::setRenderTarget( IRenderTarget* texture )
{
	CD3D9RenderTarget* tex = static_cast<CD3D9RenderTarget*>(texture);

	HRESULT hr;

	if ( tex == NULL_PTR )
	{
		hr = pID3DDevice->SetRenderTarget(0, DefaultBackBuffer);
		if(FAILED(hr))
		{
			ASSERT(false);
			return false;
		}
		hr = pID3DDevice->SetDepthStencilSurface(DefaultDepthBuffer);
		if(FAILED(hr))
		{
			ASSERT(false);
			return false;
		}
	}
	else
	{
		hr = pID3DDevice->SetRenderTarget(0, tex->getRTTSurface());
		if(FAILED(hr))
			return false;

		hr = pID3DDevice->SetDepthStencilSurface(tex->getRTTDepthSurface());
		if(FAILED(hr))
		{
			ASSERT(false);
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
	if ( vp.getWidth() <= 0 || vp.getHeight() <= 0 )
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

	Present.BackBufferWidth = ScreenSize.Width;
	Present.BackBufferHeight = ScreenSize.Height;

	reset();

	setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));
}

bool CD3D9Driver::setDriverSetting( const SDriverSetting& setting )
{
	bool vsyncChanged = false;
	bool antialiasChanged = false;
	bool fullscreenChanged = false;

	bool vsync = setting.vsync;
	u8 antialias = setting.antialias;
	bool fullscreen = setting.fullscreen;

	if (vsync != DriverSetting.vsync)
	{
		Present.PresentationInterval = vsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
		vsyncChanged = true;

		DriverSetting.vsync = vsync;
	}

	if (antialias > 0)
	{
		if(antialias > 16)
			antialias = 16;

		DWORD qualityLevels = 0;

		while(antialias > 0)
		{
			if(SUCCEEDED(pID3D->CheckDeviceMultiSampleType(AdapterInfo.index,
				DevType, Present.BackBufferFormat, !fullscreen,
				(D3DMULTISAMPLE_TYPE)antialias, &qualityLevels)) && qualityLevels)
			{
				u32 quality = min_((u32)(qualityLevels-1), 1u);
				Present.MultiSampleType	= (D3DMULTISAMPLE_TYPE)antialias;
				Present.MultiSampleQuality = quality;
				Present.SwapEffect		= D3DSWAPEFFECT_DISCARD;		//multisample requies discard
				break;
			}
			--antialias;
		}
	}

	if (antialias != DriverSetting.antialias)
	{
		antialiasChanged = true;

		DriverSetting.antialias = antialias;
		DriverSetting.quality = (u8)Present.MultiSampleQuality;
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
			Present.BackBufferWidth = windowSize.Width;
			Present.BackBufferHeight = windowSize.Height;
			Present.BackBufferFormat = d3ddm.Format;
			Present.SwapEffect	= D3DSWAPEFFECT_FLIP;
			Present.Windowed	= FALSE;
			Present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		}
		else
		{
			Present.BackBufferWidth = windowSize.Width;
			Present.BackBufferHeight =windowSize.Height;
			Present.BackBufferFormat = d3ddm.Format;
			Present.SwapEffect		= D3DSWAPEFFECT_DISCARD;
			Present.Windowed		= TRUE;
			Present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		}

		ScreenSize = windowSize;

		fullscreenChanged = true;

		DriverSetting.fullscreen = fullscreen;
	}

	bool ret = true;
	if (vsyncChanged || fullscreenChanged || antialiasChanged)
	{
// 		if (beginScene())
// 		{
// 			clear(true, false, false, SColor(0,0,0));
// 			endScene();
// 		}
		ret = reset();

		setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));

		if(ret)
		{ 	
			g_Engine->getFileSystem()->writeLog(ELOG_GX, "Driver Setting Changed. Vsync: %s, Antialias: %d, %s", 
				Present.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE ? "Off" : "On",
				(s32)DriverSetting.antialias, 
				Present.Windowed ? "Window" : "FullScreen");
		}
	}
	return ret;
}

bool CD3D9Driver::reset()
{
	for( T_LostResetList::const_iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onLost();

	//depth stencil buffer
	SAFE_RELEASE(DefaultDepthBuffer);

	//back buffer
	SAFE_RELEASE(DefaultBackBuffer);

	HRESULT hr;

	if (g_Engine->getOSInfo()->IsAeroSupport())
	{
		SDisplayMode dm = AdapterInfo.getCloseMatchDisplayMode(Present.BackBufferWidth, Present.BackBufferHeight);

		if (!Present.Windowed)		//adjust
		{
			Present.BackBufferWidth = dm.width;
			Present.BackBufferHeight = dm.height;
		}

		D3DDISPLAYMODEEX displayMode;
		displayMode.Size = sizeof(D3DDISPLAYMODEEX);
		displayMode.Width = dm.width;
		displayMode.Height = dm.height;
		displayMode.RefreshRate = dm.refreshrate;
		displayMode.Format = Present.BackBufferFormat;
		displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		hr = ((IDirect3DDevice9Ex*)pID3DDevice)->ResetEx(&Present, Present.Windowed ? NULL_PTR : &displayMode);

		if (!Present.Windowed)		//adjust
		{
			ScreenSize.Width = dm.width;
			ScreenSize.Height = dm.height;
		}
	}
	else
	{
		SDisplayMode dm = AdapterInfo.getCloseMatchDisplayMode(Present.BackBufferWidth, Present.BackBufferHeight);

		if (!Present.Windowed)		//adjust
		{
			Present.BackBufferWidth = dm.width;
			Present.BackBufferHeight = dm.height;
		}

		hr = pID3DDevice->Reset(&Present);

		if (!Present.Windowed)		//adjust
		{
			ScreenSize.Width = dm.width;
			ScreenSize.Height = dm.height;
		}
	}

	if ( FAILED(hr) )
	{
		if (hr == D3DERR_DEVICELOST)
			DeviceLost = true;
		else
		{
			//	ASSERT(hr == D3DERR_DRIVERINTERNALERROR);
			ASSERT(false);					//有资源没有lost, reset
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

	DeviceLost = false;
	
	for( T_LostResetList::const_iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onReset();

	//reset
	LastMaterial.MaterialType = (E_MATERIAL_TYPE)-1;
	ResetRenderStates = true;
	CurrentRenderMode = ERM_NONE;
	CurrentDeviceState.reset();
	
	D3D9SceneStateServices->reset();

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "reset... Device Format: %dX%d, %s", Present.BackBufferWidth, Present.BackBufferHeight, CD3D9Helper::getD3DFormatString(Present.BackBufferFormat));

	return true;
}

void CD3D9Driver::createVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		VertexDeclarations[i] = new CD3D9VertexDeclaration((E_VERTEX_TYPE)i);
	}
}

void CD3D9Driver::releaseVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		delete VertexDeclarations[i];
	}
}

void CD3D9Driver::setVertexDeclaration( E_VERTEX_TYPE type )
{
	if (CurrentDeviceState.vType != type)
	{
		CD3D9VertexDeclaration* decl = getVertexDeclaration(type);
		ASSERT(decl);
		pID3DDevice->SetVertexDeclaration(decl->getDx9Declaration());
		CurrentDeviceState.vType = type;
	}
}

void CD3D9Driver::registerLostReset( ILostResetCallback* callback )
{
	if (std::find(LostResetList.begin(), LostResetList.end(), callback) == LostResetList.end())
	{
		LostResetList.push_back(callback);
	}
}

void CD3D9Driver::removeLostReset( ILostResetCallback* callback )
{
	LostResetList.remove(callback);
}

#endif
