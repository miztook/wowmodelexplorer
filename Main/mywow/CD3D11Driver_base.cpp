#include "stdafx.h"
#include "CD3D11Driver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11SceneStateServices.h"
#include "CD3D11MaterialRenderServices.h"
#include "CD3D11ShaderServices.h"
#include "CD3D11VertexDeclaration.h"
#include "CD3D11Helper.h"
#include "CD3D11RenderTarget.h"


CD3D11Driver::CD3D11Driver()
{
	HLibDXGI = NULL_PTR;
	HLibD3D = NULL_PTR;
	pID3DDevice11 = NULL_PTR;
	ImmediateContext = NULL_PTR;

	HWnd = 0;
	DevType = D3D_DRIVER_TYPE_HARDWARE;
	
	AdapterCount = 0;
	SwapChain = NULL_PTR;
	Adapter = NULL_PTR;
	DXGIOutput = NULL_PTR;
	DXGIFactory = NULL_PTR;

	DefaultBackBuffer = NULL_PTR;
	DefaultDepthTexture = NULL_PTR;
	DefaultDepthBuffer = NULL_PTR;
	BackBufferFormat = DXGI_FORMAT_UNKNOWN;
	DepthStencilFormat = DXGI_FORMAT_UNKNOWN;
	DepthTextureFormat = DXGI_FORMAT_UNKNOWN;
	DepthSRFormat = DXGI_FORMAT_UNKNOWN;

	PrimitivesDrawn = 0;
	DrawCall = 0;

	CurrentDeviceState.reset();
	CurrentRenderMode = ERM_NONE;
	ResetRenderStates = true;
	CurrentRenderTarget = NULL_PTR;

	MaterialRenderer = NULL_PTR;
	ShaderServices = NULL_PTR;
	MaterialRenderServices = NULL_PTR;
	SceneStateServices = NULL_PTR;

	D3D11ShaderServices = NULL_PTR;
	D3D11MaterialRenderServices = NULL_PTR;
	D3D11SceneStateServices = NULL_PTR;

	//2D
	InitMaterial2D.MaterialType = EMT_2D;
	InitMaterial2D.Cull = ECM_BACK;
	InitMaterial2D.Lighting = false;
	InitMaterial2D.ZWriteEnable = false;
	InitMaterial2D.AntiAliasing = EAAM_OFF;
	InitMaterial2D.ZBuffer = ECFN_NEVER;
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

	FeatureLevel = D3D_FEATURE_LEVEL_10_0;
	
	memset(&Present, 0, sizeof(Present));
	memset(VertexDeclarations, 0, sizeof(VertexDeclarations));
	memset(DebugMsg, 0, sizeof(DebugMsg));
}

CD3D11Driver::~CD3D11Driver()
{
	if (SwapChain)
		SwapChain->SetFullscreenState(FALSE, NULL_PTR);
	if (ImmediateContext)
	{
		ImmediateContext->ClearState();
		ImmediateContext->Flush();
	}

	releaseVertexDecl();

 	delete		SceneStateServices;
	delete		MaterialRenderServices;
 	delete		ShaderServices;

	SAFE_RELEASE_STRICT(DefaultBackBuffer);
	SAFE_RELEASE_STRICT(DefaultDepthBuffer);
	SAFE_RELEASE_STRICT(DefaultDepthTexture);

	if (SwapChain)
		SwapChain->Release();
	if (DXGIOutput)
		DXGIOutput->Release();
	if (DXGIFactory)
		DXGIFactory->Release();
	if(Adapter)
		Adapter->Release();
	if (ImmediateContext)
		ImmediateContext->Release();

	if (pID3DDevice11)
		pID3DDevice11->Release();

	if (HLibD3D)
		FreeLibrary(HLibD3D);
}

bool CD3D11Driver::initDriver( const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{
	ASSERT(::IsWindow(wndInfo.hwnd));

	HWnd = wndInfo.hwnd;
	DriverSetting.vsync = vsync;
	DriverSetting.fullscreen = fullscreen;

	RECT rc;
	::GetClientRect(wndInfo.hwnd, &rc);
	dimension2du windowSize((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);

	HLibDXGI = ::LoadLibraryA("dxgi.dll");
	HLibD3D = ::LoadLibraryA("d3d11.dll");
	if (!HLibDXGI || !HLibD3D)
	{
		ASSERT(false);
		return false;
	}

	HRESULT hr;
	PFN_D3D11_CREATE_DEVICE createDeviceFunc = (PFN_D3D11_CREATE_DEVICE) GetProcAddress( HLibD3D, "D3D11CreateDevice" );
	if(!createDeviceFunc) { ASSERT(false); return false; }

	AdapterCount = 1;
	AdapterInfo.index = adapter >= AdapterCount ? 0 : adapter;

	D3D_FEATURE_LEVEL RequestedLevels[] = 
	{  
		D3D_FEATURE_LEVEL_11_0, 
		D3D_FEATURE_LEVEL_10_1, 
		D3D_FEATURE_LEVEL_10_0, 
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	u32 RequestedLevelsSize = sizeof( RequestedLevels ) / sizeof( RequestedLevels[0] );

	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG) || defined(DEBUG)
 	//flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if(!multithread)
		flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;

	hr = createDeviceFunc( NULL_PTR, DevType, NULL_PTR, flags, RequestedLevels, RequestedLevelsSize, D3D11_SDK_VERSION, &pID3DDevice11, &FeatureLevel, &ImmediateContext );
	if (FAILED(hr)) 
	{ 
		ASSERT(false); 
		return false; 
	}

	switch(FeatureLevel)
	{
	case D3D_FEATURE_LEVEL_11_0:
		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX11");
		break;
	case D3D_FEATURE_LEVEL_10_1:
		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX10.1");
		break;
	case D3D_FEATURE_LEVEL_10_0:
		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX10");
		break;
	case D3D_FEATURE_LEVEL_9_3:
		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX9.3");
		break;
	case D3D_FEATURE_LEVEL_9_2:
		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX9.2");
		break;
	case D3D_FEATURE_LEVEL_9_1:
		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: DirectX9.1");
		break;
	default:
		g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: Unknown");
		break;
	}

	IDXGIDevice* DXGIDevice = NULL_PTR;
	pID3DDevice11->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast<void**>( &DXGIDevice ) );

	//adapter, factory, output
	hr = DXGIDevice->GetAdapter( &Adapter );
	if (FAILED(hr))  { ASSERT(false); return false; }

	hr = Adapter->GetParent( __uuidof( IDXGIFactory ), reinterpret_cast<void**>( &DXGIFactory ) );
	if (FAILED(hr))  { ASSERT(false); return false; }

	DXGI_ADAPTER_DESC adapDesc;
	::ZeroMemory( &adapDesc, sizeof( DXGI_ADAPTER_DESC ) );
	Adapter->GetDesc( &adapDesc );

	hr = Adapter->EnumOutputs( 0, &DXGIOutput);
	if(FAILED(hr)) { ASSERT(false); return false; }

	DXGI_OUTPUT_DESC outputDesc;
	::ZeroMemory( &outputDesc, sizeof(DXGI_OUTPUT_DESC) );
	DXGIOutput->GetDesc(&outputDesc);

	utf16to8( (const c16*)adapDesc.Description, AdapterInfo.description, 512 );
	utf16to8( (const c16*)outputDesc.DeviceName, AdapterInfo.name, DEFAULT_SIZE);

	AdapterInfo.vendorID = adapDesc.VendorId;
	switch (adapDesc.VendorId)
	{
	case 0x1002: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "ATI Technologies Inc."); break;
	case 0x10DE: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "NVIDIA Corporation"); break;
	case 0x102B: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "Matrox Electronic System Ltd."); break;
	case 0x121A: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "3dfx Interactive Inc"); break;
	case 0x5333: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "S3 Graphics Co. Ltd"); break;
	case 0x8086: Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, "Intel Corporation"); break;
	default:
		Q_sprintf(AdapterInfo.vendorName, DEFAULT_SIZE, "Unknown VendorId: %x", adapDesc.VendorId);
		break;
	}

	// set present params...
	u32 numModes = 0;
	hr = DXGIOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_SCALING, &numModes, NULL_PTR);
	ASSERT(SUCCEEDED(hr));

	DXGI_MODE_DESC* pDesc = new DXGI_MODE_DESC[numModes];
	hr = DXGIOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_SCALING, &numModes, pDesc);
	ASSERT(SUCCEEDED(hr));
	for (u32 i=0; i<numModes; ++i)
	{
		SDisplayMode displayMode;
		displayMode.width = (u16)pDesc[i].Width;
		displayMode.height = (u16)pDesc[i].Height;
		displayMode.refreshrate = 60;
		AdapterInfo.addDisplayMode(displayMode);
	}
	delete[] pDesc;

	//log
	g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Adapter Name: %s", AdapterInfo.name);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Adpater Description: %s", AdapterInfo.description);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Adapter Vendor: %s", AdapterInfo.vendorName);

	ZeroMemory(&Present, sizeof(Present));
	Present.BufferCount = 1;
	Present.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Present.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Present.OutputWindow = HWnd;

	if (vsync)
		Present.BufferDesc.RefreshRate.Numerator = 0;
	else
		Present.BufferDesc.RefreshRate.Numerator = 60;
	Present.BufferDesc.RefreshRate.Denominator = 1;

	if (fullscreen)
	{
		Present.BufferDesc.Width = (UINT)windowSize.Width;
		Present.BufferDesc.Height = (UINT)windowSize.Height;
		Present.Windowed = false;	
		Present.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

		Present.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
		Present.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST;
		Present.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;

		//adjust
		DXGI_MODE_DESC* request = &Present.BufferDesc;
		DXGI_MODE_DESC match;
		ZeroMemory( &match, sizeof( DXGI_MODE_DESC ) );
		hr = DXGIOutput->FindClosestMatchingMode( request, &match, pID3DDevice11 );
		if( SUCCEEDED(hr) ) 
		{
			Present.BufferDesc.Format = match.Format;
			Present.BufferDesc.Height = match.Height;
			Present.BufferDesc.RefreshRate = match.RefreshRate;
			Present.BufferDesc.Scaling = match.Scaling;
			Present.BufferDesc.ScanlineOrdering = match.ScanlineOrdering;
			Present.BufferDesc.Width = match.Width;

			// Adjust screen size accordingly
			windowSize.Width = match.Width;
			windowSize.Height = match.Height;
		}
	}
	else
	{
		Present.BufferDesc.Width = (UINT)windowSize.Width;
		Present.BufferDesc.Height = (UINT)windowSize.Height;
		Present.Windowed = TRUE;
		Present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	//antialias
	if (antialias > 0)
	{
		if(antialias > 16)
			antialias = 16;

		UINT qualityLevels = 0;

		while(antialias > 0)
		{
			if(SUCCEEDED(pID3DDevice11->CheckMultisampleQualityLevels(Present.BufferDesc.Format,
				antialias * 2, &qualityLevels)) && qualityLevels)
			{
				u32 quality = min_((u32)(qualityLevels-1), 1u);
				Present.SampleDesc.Count	= (UINT)antialias * 2;
				Present.SampleDesc.Quality = quality;
				Present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;		//requies discard
				break;
			}
			--antialias;
		}
	}
	DriverSetting.antialias = antialias;
	DriverSetting.quality = (u8)Present.SampleDesc.Quality;

	DXGIFactory->MakeWindowAssociation(HWnd, 0);

	//swap chain
	hr = DXGIFactory->CreateSwapChain(pID3DDevice11, &Present, &SwapChain);
	ASSERT(SUCCEEDED(hr));

	if (!SwapChain) { ASSERT(false); return false; }
	DXGIDevice->Release();

	// Get default render target
	ID3D11Texture2D* backBuffer = NULL_PTR;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) );
	if( FAILED(hr))
	{
		ASSERT(false);
		return false;
	}
	D3D11_TEXTURE2D_DESC texDesc;
	backBuffer->GetDesc(&texDesc);
	BackBufferFormat = texDesc.Format;
	hr = pID3DDevice11->CreateRenderTargetView( backBuffer, NULL_PTR, &DefaultBackBuffer );
	if( FAILED(hr))
	{
		ASSERT(false);
		return false;
	}
	backBuffer->Release();

	//depth buffer
	recreateDepthStencilView(windowSize, ECF_D24S8, DriverSetting.antialias, DriverSetting.quality);

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Device Format: %dX%d, %s, %s", Present.BufferDesc.Width, Present.BufferDesc.Height, Present.Windowed ? "Window" : "FullScreen", CD3D11Helper::getD3DFormatString(Present.BufferDesc.Format));
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Device Vsync: %s, MultiSample: %d, DepthStencil Format:%s", 
		Present.BufferDesc.RefreshRate.Numerator == 60 ? "Off" : "On",
		(int)Present.SampleDesc.Count / 2, CD3D11Helper::getD3DFormatString(DepthStencilFormat));

	// Set render targets
	ImmediateContext->OMSetRenderTargets( 1, &DefaultBackBuffer, DefaultDepthBuffer );

	//log 
	logCaps();

	if (!queryFeature(EVDF_PIXEL_SHADER_5_0) &&
		!queryFeature(EVDF_PIXEL_SHADER_4_0) )
	{
		CSysUtility::messageBoxWarning("Video Card does not support some features. Application will now exit!");
		return false;
	}

	//shader, material
 	ShaderServices =
		D3D11ShaderServices = new CD3D11ShaderServices;
	MaterialRenderServices = 
		D3D11MaterialRenderServices = new CD3D11MaterialRenderServices;
	SceneStateServices =
		D3D11SceneStateServices = new CD3D11SceneStateServices;

	createVertexDecl();	

	setDisplayMode(windowSize);

	return true;
}

void CD3D11Driver::recreateDepthStencilView( dimension2du size, ECOLOR_FORMAT depthFmt, u32 antialias, u32 quality )
{
	SAFE_RELEASE_STRICT(DefaultDepthBuffer);
	SAFE_RELEASE_STRICT(DefaultDepthTexture);

	UINT formatSupport = 0;
	HRESULT hr;

	DepthStencilFormat = CD3D11Helper::getDXGIFormatFromColorFormat(depthFmt, DepthTextureFormat, DepthSRFormat);

	if (SUCCEEDED(pID3DDevice11->CheckFormatSupport( DepthStencilFormat, &formatSupport )))
	{
		if( !(formatSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL) )
		{
			ASSERT(false);
			return;
		}
	}
	else
	{
		ASSERT(false);
		return;
	}

	// create depth buffer
	ID3D11DepthStencilView* dsView = NULL_PTR;
	ID3D11Texture2D* depthTexture = NULL_PTR;
	D3D11_TEXTURE2D_DESC dsTexDesc;
	::ZeroMemory( &dsTexDesc, sizeof( dsTexDesc ) );
	dsTexDesc.ArraySize = 1;
	dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsTexDesc.Format = DepthStencilFormat;
	dsTexDesc.MipLevels = 1;
	dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTexDesc.SampleDesc.Count = antialias * 2;
	dsTexDesc.SampleDesc.Quality = quality;
	dsTexDesc.Width = (UINT)size.Width;
	dsTexDesc.Height = (UINT)size.Height;
	hr = pID3DDevice11->CreateTexture2D( &dsTexDesc, NULL_PTR, &depthTexture );
	if(FAILED(hr))
	{
		ASSERT(false);
		DefaultDepthBuffer = NULL_PTR;
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	::ZeroMemory( &dsDesc, sizeof( dsDesc ) );
	dsDesc.Format = DepthStencilFormat;
	dsDesc.Flags = 0;
	dsDesc.ViewDimension = antialias * 2 > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	hr = pID3DDevice11->CreateDepthStencilView( depthTexture, &dsDesc, &dsView );
	DefaultDepthTexture = depthTexture;

	if(FAILED(hr))
	{
		ASSERT(false);
		SAFE_RELEASE_STRICT(DefaultDepthTexture);
		DefaultDepthBuffer = NULL_PTR;
		return;
	}

	DefaultDepthBuffer = dsView;
}

void CD3D11Driver::logCaps()
{
	IFileSystem* fs = g_Engine->getFileSystem();

	if (queryFeature(EVDF_PIXEL_SHADER_5_0))
		fs->writeLog(ELOG_GX, "\tPixel Shader: 5.0");
	else if (queryFeature(EVDF_PIXEL_SHADER_4_0))
		fs->writeLog(ELOG_GX, "\tPixel Shader: 4.0");
	else
		fs->writeLog(ELOG_GX, "\tPixel Shader 4.0 minimum not supported!");

	if (queryFeature(EVDF_VERTEX_SHADER_5_0))
		fs->writeLog(ELOG_GX, "\tVertex Shader: 5.0");
	else if (queryFeature(EVDF_VERTEX_SHADER_4_0))
		fs->writeLog(ELOG_GX, "\tVertex Shader: 4.0");
	else
		fs->writeLog(ELOG_GX, "\tVertex Shader 4.0 minimum not supported!");
}

bool CD3D11Driver::beginScene()
{
	PrimitivesDrawn = 0;
	DrawCall = 0;

	//ImmediateContext
	{
		ID3D11ShaderResourceView* views[1] = { NULL_PTR };

		ImmediateContext->VSSetShaderResources( 0, 1, views );
		ImmediateContext->GSSetShaderResources( 0, 1, views );
		ImmediateContext->PSSetShaderResources( 0, 1, views );
	}

	return true;
}

bool CD3D11Driver::endScene()
{
	HRESULT hr = SwapChain->Present(DriverSetting.vsync ? 1 : 0, 0);
	if (FAILED(hr))
	{
		ASSERT(false);
		return false;
	}
	return true;
}

bool CD3D11Driver::clear( bool renderTarget, bool zBuffer, bool stencil, SColor color )
{
	if (renderTarget)
	{	
		//ImmediateContext
		SColorf fCol( color );
		FLOAT c[4] = { fCol.r, fCol.g, fCol.b, fCol.a };	

		CD3D11RenderTarget* rt = static_cast<CD3D11RenderTarget*>(CurrentRenderTarget);
		if (rt)
			ImmediateContext->ClearRenderTargetView( rt->getRenderTargetView(), c);
		else
			ImmediateContext->ClearRenderTargetView( DefaultBackBuffer, c );
	}

	if (zBuffer || stencil)
	{
		UINT flags = 0;
		if (zBuffer)
			flags |= D3D11_CLEAR_DEPTH;
		if (stencil)
			flags |= D3D11_CLEAR_STENCIL;

		CD3D11RenderTarget* rt = static_cast<CD3D11RenderTarget*>(CurrentRenderTarget);
		if (rt)
			ImmediateContext->ClearDepthStencilView( rt->getDepthStencilView(), flags, 1.0f, 0 );
		else
			ImmediateContext->ClearDepthStencilView( DefaultDepthBuffer, flags, 1.0f, 0 );
	}

	return true;
}

bool CD3D11Driver::queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
{
	switch (feature)
	{
	case EVDF_RENDER_TO_TARGET:
	case EVDF_HARDWARE_TL:
	case EVDF_TEXTURE_ADDRESS:
	case EVDF_SEPARATE_UVWRAP:
	case  EVDF_MIP_MAP:
	case EVDF_STENCIL_BUFFER:
	case EVDF_TEXTURE_NSQUARE:
	case EVDF_TEXTURE_NPOT:
	case EVDF_COLOR_MASK:
	case EVDF_MULTIPLE_RENDER_TARGETS:
	case EVDF_MRT_COLOR_MASK:
	case EVDF_MRT_BLEND_FUNC:
	case EVDF_MRT_BLEND:
	case EVDF_STREAM_OFFSET:
	case	EVDF_TEXTURE_MULTISAMPLING:
		return true;
	case EVDF_PIXEL_SHADER_2_0:
	case EVDF_VERTEX_SHADER_2_0:
		return FeatureLevel == D3D_FEATURE_LEVEL_9_1 ||
			FeatureLevel == D3D_FEATURE_LEVEL_9_2;

	case EVDF_PIXEL_SHADER_3_0:
	case EVDF_VERTEX_SHADER_3_0:
		return FeatureLevel == D3D_FEATURE_LEVEL_9_3;

	case EVDF_VERTEX_SHADER_4_0:
	case EVDF_PIXEL_SHADER_4_0:
	case EVDF_GEOMETRY_SHADER_4_0:
	case EVDF_STREAM_OUTPUT_4_0:
		return FeatureLevel == D3D_FEATURE_LEVEL_10_0 ||
			FeatureLevel == D3D_FEATURE_LEVEL_10_1;

	case EVDF_VERTEX_SHADER_4_1:
	case EVDF_PIXEL_SHADER_4_1:
	case EVDF_GEOMETRY_SHADER_4_1:
	case EVDF_STREAM_OUTPUT_4_1:
		return FeatureLevel == D3D_FEATURE_LEVEL_10_1;

	case EVDF_VERTEX_SHADER_5_0:
	case EVDF_PIXEL_SHADER_5_0:
	case EVDF_GEOMETRY_SHADER_5_0:
	case EVDF_STREAM_OUTPUT_5_0:
	case EVDF_TESSELATION_SHADER_5_0:
		return FeatureLevel == D3D_FEATURE_LEVEL_11_0;

	case EVDF_COMPUTING_SHADER_4_0:
	case EVDF_COMPUTING_SHADER_4_1:
		{
			D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS opts;
			if (SUCCEEDED( pID3DDevice11->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, 
				&opts, sizeof(D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS)) ))
			{
				return opts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x != 0;
			}
			return false;
		}

	case EVDF_COMPUTING_SHADER_5_0:
		return FeatureLevel == D3D_FEATURE_LEVEL_11_0;
	}

	return false;
}

bool CD3D11Driver::reset(bool recreateSwapChain)
{
	for( T_LostResetList::const_iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onLost();

	//ImmediateContext
	ID3D11RenderTargetView* views[1] = { NULL_PTR };	
	ImmediateContext->OMSetRenderTargets(1, views, NULL_PTR);

	SAFE_RELEASE_STRICT(DefaultDepthBuffer);
	SAFE_RELEASE_STRICT(DefaultDepthTexture);
	SAFE_RELEASE_STRICT(DefaultBackBuffer);

	//ImmediateContext
	ImmediateContext->ClearState();

	if (DriverSetting.fullscreen)
	{
		SwapChain->SetFullscreenState( TRUE, DXGIOutput );
	}

	HRESULT hr;
	if (!recreateSwapChain)
	{
		hr = SwapChain->ResizeBuffers( 1, ScreenSize.Width, ScreenSize.Height, Present.BufferDesc.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
		if (FAILED(hr))
		{
			ASSERT(false);
			return false;
		}
	}
	else
	{
		SAFE_RELEASE_STRICT(SwapChain);

		hr = DXGIFactory->CreateSwapChain(pID3DDevice11, &Present, &SwapChain);
		ASSERT(SUCCEEDED(hr));

		if (!SwapChain) { ASSERT(false); return false; }
	}

	// Get default render target
	ID3D11Texture2D* backBuffer = NULL_PTR;
	hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) );
	if( FAILED(hr))
	{
		ASSERT(false);
		return false;
	}
	D3D11_TEXTURE2D_DESC texDesc;
	backBuffer->GetDesc(&texDesc);
	BackBufferFormat = texDesc.Format;
	hr = pID3DDevice11->CreateRenderTargetView( backBuffer, NULL_PTR, &DefaultBackBuffer );
	SAFE_RELEASE_STRICT(backBuffer);
	if( FAILED(hr))
	{
		ASSERT(false);
		return false;
	}

	//depth buffer
	recreateDepthStencilView(ScreenSize, ECF_D24S8, DriverSetting.antialias, DriverSetting.quality);

	// Set render targets
	//ImmediateContext
	ImmediateContext->OMSetRenderTargets( 1, &DefaultBackBuffer, DefaultDepthBuffer );

	for( T_LostResetList::const_iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onReset();

	//reset
	LastMaterial.MaterialType = (E_MATERIAL_TYPE)-1;
	ResetRenderStates = true;
	CurrentRenderMode = ERM_NONE;
	CurrentDeviceState.reset();

	D3D11SceneStateServices->reset();

	return true;
}

void CD3D11Driver::setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat )
{
	ASSERT( 0 <= state && state < ETS_COUNT );

	switch( state )
	{
	case ETS_VIEW:
		Matrices[ETS_VIEW] = mat;

		WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
		WVP = WV * Matrices[ETS_PROJECTION];

		CurrentRenderMode = ERM_3D;

		break;
	case ETS_WORLD:
		Matrices[ETS_WORLD] = mat;

		WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
		WVP = WV * Matrices[ETS_PROJECTION];

		CurrentRenderMode = ERM_3D;

		break;
	case ETS_PROJECTION:
		Matrices[ETS_PROJECTION] = mat;

		WVP = WV * Matrices[ETS_PROJECTION];

		CurrentRenderMode = ERM_3D;

		break;
	default:		//texture
		{
			s32 index = state - ETS_TEXTURE_0;
			if (index >= 0 && index < MATERIAL_MAX_TEXTURES)
			{
				Matrices[index] = mat;
			}
		}
		break;
	}
}

void CD3D11Driver::setTexture( u32 stage, ITexture* texture )
{
	D3D11MaterialRenderServices->setSampler_Texture(stage, texture);
}

void CD3D11Driver::setTransform(const matrix4& matView, const matrix4& matProjection)
{
	Matrices[ETS_VIEW] = matView;
	Matrices[ETS_PROJECTION] = matProjection;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;
}

void CD3D11Driver::setTransform_Material_Textures( const matrix4& matWorld, const SMaterial& material, ITexture* const textures[], u32 numTextures )
{
	Matrices[ETS_WORLD] = matWorld;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;

	Material = material;

	u32 n = min_(numTextures, (u32)MATERIAL_MAX_TEXTURES);
	for (u32 i=0; i<n; ++i)
	{
		D3D11MaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

void CD3D11Driver::setTransform_Material_Textures( const matrix4& matWorld, const matrix4& matView, const matrix4& matProjection, const SMaterial& material, ITexture* const textures[], u32 numTextures )
{
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
		D3D11MaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

ITexture* CD3D11Driver::getTexture( u32 index ) const
{
	return D3D11MaterialRenderServices->getSampler_Texture(index);
}

bool CD3D11Driver::setRenderTarget( IRenderTarget* texture )
{
	CD3D11RenderTarget* tex = static_cast<CD3D11RenderTarget*>(texture);

	if ( tex == NULL_PTR )
	{
		ImmediateContext->OMSetRenderTargets( 1, &DefaultBackBuffer, DefaultDepthBuffer );
	}
	else
	{
		//ImmediateContext
		{
			ID3D11ShaderResourceView* views[1] = { NULL_PTR };

			ImmediateContext->VSSetShaderResources( 0, 1, views );
			ImmediateContext->GSSetShaderResources( 0, 1, views );
			ImmediateContext->PSSetShaderResources( 0, 1, views );
		}

		ID3D11RenderTargetView*  rtViews[1] = { NULL_PTR };
		rtViews[0] = tex->getRenderTargetView();
		ImmediateContext->OMSetRenderTargets( 1, rtViews, tex->getDepthStencilView() );
	}

	CurrentRenderTarget = texture;

	return true;
}

void CD3D11Driver::setViewPort( recti area )
{
	recti vp = area;
	vp.clipAgainst(recti(0,0,ScreenSize.Width,ScreenSize.Height));
	if ( vp.getWidth() <= 0 || vp.getHeight() <= 0 )
		return;

	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = (FLOAT)vp.UpperLeftCorner.X;
	viewPort.TopLeftY = (FLOAT)vp.UpperLeftCorner.Y;
	viewPort.Width = (FLOAT)vp.getWidth();
	viewPort.Height = (FLOAT)vp.getHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	//ImmediateContext
	ImmediateContext->RSSetViewports( 1, &viewPort );

	Viewport = vp;
}

void CD3D11Driver::setDisplayMode( const dimension2du& size )
{
	ScreenSize = size;

	Present.BufferDesc.Width = ScreenSize.Width;
	Present.BufferDesc.Height = ScreenSize.Height;

	reset(false);

	setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));
}

bool CD3D11Driver::setDriverSetting( const SDriverSetting& setting )
{
	bool change = false;
	bool vsync = setting.vsync;
	u8 antialias = setting.antialias;
	bool fullscreen = setting.fullscreen;

	if (vsync != DriverSetting.vsync)
	{
		if (vsync)
			Present.BufferDesc.RefreshRate.Numerator = 0;
		else
			Present.BufferDesc.RefreshRate.Numerator = 60;
		Present.BufferDesc.RefreshRate.Denominator = 1;
		change = true;

		DriverSetting.vsync = vsync;
	}
	
	if (fullscreen != DriverSetting.fullscreen)
	{
		RECT rc;
		::GetClientRect(HWnd, &rc);
		dimension2du windowSize((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);

		if (fullscreen)
		{
			Present.BufferDesc.Width = (UINT)windowSize.Width;
			Present.BufferDesc.Height = (UINT)windowSize.Height;
			Present.Windowed = false;	
			Present.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

			Present.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
			Present.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST;
			Present.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;

			//adjust
			DXGI_MODE_DESC* request = &Present.BufferDesc;
			DXGI_MODE_DESC match;
			ZeroMemory( &match, sizeof( DXGI_MODE_DESC ) );
			HRESULT hr = DXGIOutput->FindClosestMatchingMode( request, &match, pID3DDevice11 );
			if( SUCCEEDED(hr) ) 
			{
				Present.BufferDesc.Format = match.Format;
				Present.BufferDesc.Height = match.Height;
				Present.BufferDesc.RefreshRate = match.RefreshRate;
				Present.BufferDesc.Scaling = match.Scaling;
				Present.BufferDesc.ScanlineOrdering = match.ScanlineOrdering;
				Present.BufferDesc.Width = match.Width;

				// Adjust screen size accordingly
				windowSize.Width = match.Width;
				windowSize.Height = match.Height;
			}
		}
		else
		{
			Present.BufferDesc.Width = (UINT)windowSize.Width;
			Present.BufferDesc.Height = (UINT)windowSize.Height;
			Present.Windowed = true;
			Present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		}
		ScreenSize = windowSize;

		DriverSetting.fullscreen = fullscreen;
	}
	
	//antialias
	if (antialias > 0)
	{
		if(antialias > 16)
			antialias = 16;

		UINT qualityLevels = 0;

		while(antialias > 0)
		{
			if(SUCCEEDED(pID3DDevice11->CheckMultisampleQualityLevels(Present.BufferDesc.Format,
				antialias * 2, &qualityLevels)) && qualityLevels)
			{
				u8 quality = min_((u32)(qualityLevels-1), 1u);
				Present.SampleDesc.Count	= antialias * 2;
				Present.SampleDesc.Quality = quality;
				Present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;		//requies discard
				break;
			}
			--antialias;
		}
	}

	if (antialias != DriverSetting.antialias)
	{
		change = true;

		DriverSetting.antialias = antialias;
		DriverSetting.quality = Present.SampleDesc.Quality;
	}

	bool ret = true;
	if (change)
	{
		
		ret = reset(true);
		setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));

		if(ret)
		{ 	
			g_Engine->getFileSystem()->writeLog(ELOG_GX, "Driver Setting Changed. Vsync: %s, Antialias: %d, %s", 
				Present.BufferDesc.RefreshRate.Numerator == 60 ? "Off" : "On",
				(s32)DriverSetting.antialias, 
				Present.Windowed ? "Window" : "FullScreen");
		}
	}
	return ret;
}

void CD3D11Driver::createVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		VertexDeclarations[i] = new CD3D11VertexDeclaration((E_VERTEX_TYPE)i);
	}
}

void CD3D11Driver::releaseVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		delete VertexDeclarations[i];
	}
}

void CD3D11Driver::setVertexDeclaration( E_VERTEX_TYPE type, IVertexShader* vs )
{
	if (CurrentDeviceState.vType != type)
	{
		CD3D11VertexDeclaration* decl = getVertexDeclaration(type);
		ID3D11InputLayout* inputLayout = decl->getInputLayout(vs);

		//ImmediateContext
		ImmediateContext->IASetInputLayout(inputLayout);

		CurrentDeviceState.vType = type;
	}
}

void CD3D11Driver::registerLostReset( ILostResetCallback* callback )
{
	if (std::find(LostResetList.begin(), LostResetList.end(), callback) == LostResetList.end())
	{
		LostResetList.push_back(callback);
	}
}

void CD3D11Driver::removeLostReset( ILostResetCallback* callback )
{
	LostResetList.remove(callback);
}

#endif