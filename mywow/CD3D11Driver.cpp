#include "stdafx.h"
#include "CD3D11Driver.h"
#include "mywow.h"

CD3D11Driver::CD3D11Driver()
{
	m_hD3DLib = NULL;
	pID3DDevice11 = NULL;
	ImmediateContext = NULL;

	SwapChain = NULL;
	Adapter = NULL;
	DXGIOutput = NULL;
	DXGIFactory = NULL;

	AntiAliasing = 0;
	MaxTextureUnits = 0;
	MaxUserClipPlanes = 0;
	VendorID = 0;
	AlphaToCoverageSupport = false;
	CSAAApplied = false;

	CurrentRenderMode = ERM_NONE;
	ResetRenderStates = true;

	ShaderServices = NULL;
	MaterialRenderServices = NULL;
}

CD3D11Driver::~CD3D11Driver()
{
	delete		MaterialRenderServices;
	delete		ShaderServices;

	if (SwapChain)
		SwapChain->SetFullscreenState(FALSE, NULL);
	if (ImmediateContext)
	{
		ImmediateContext->ClearState();
		ImmediateContext->Flush();
	}

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
	if(pID3DDevice11)
		pID3DDevice11->Release();

	if (m_hD3DLib)
		FreeLibrary(m_hD3DLib);
}

bool CD3D11Driver::initDriver( HWND hwnd, bool fullscreen, bool vsync, bool multithread, u8 antialias, E_CSAA csaa )
{
	_ASSERT(::IsWindow(hwnd));

	RECT rc;
	::GetClientRect(hwnd, &rc);
	dimension2du windowSize((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);

	HRESULT hr;

	m_hD3DLib = LoadLibraryA( "d3d11.dll" );
	if ( !m_hD3DLib) { _ASSERT(false); return false; }

	//device flags
	UINT deviceFlags = 0;
#ifdef _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	if (!multithread)
	{
		deviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	}

	PFN_D3D11_CREATE_DEVICE d3dCreate = (PFN_D3D11_CREATE_DEVICE)::GetProcAddress(m_hD3DLib, "D3D11CreateDevice");
	if (!d3dCreate) { _ASSERT(false); return false; }

	D3D_FEATURE_LEVEL RequesetedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	hr = d3dCreate( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, RequesetedLevels, 3, 
		D3D11_SDK_VERSION, &pID3DDevice11, &FeatureLevel, &ImmediateContext);
	if(FAILED(hr)) { _ASSERT(false); return false; }

	//adapter, DXGI factory, output
	IDXGIDevice* DXGIDevice = NULL;
	pID3DDevice11->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast<void**>( &DXGIDevice ) );
	hr = DXGIDevice->GetAdapter( &Adapter );
	if(FAILED(hr)) { _ASSERT(false); return false; }

	DXGI_ADAPTER_DESC adapDesc;
	::ZeroMemory( &adapDesc, sizeof( DXGI_ADAPTER_DESC ) );
	Adapter->GetDesc( &adapDesc );

	hr = Adapter->GetParent( __uuidof( IDXGIFactory ), reinterpret_cast<void**>( &DXGIFactory ) );
	if(FAILED(hr)) { _ASSERT(false); return false; }

	hr = Adapter->EnumOutputs( 0, &DXGIOutput);
	if(FAILED(hr)) { _ASSERT(false); return false; }

	DXGI_OUTPUT_DESC outputDesc;
	::ZeroMemory( &outputDesc, sizeof(DXGI_OUTPUT_DESC) );
	DXGIOutput->GetDesc(&outputDesc);

	wcscpy_s( DeviceDescription, 512,  adapDesc.Description );
	wcscpy_s( DeviceName, 32,  outputDesc.DeviceName );

	VendorID = adapDesc.VendorId;
	switch (adapDesc.VendorId)
	{
	case 0x1002: strcpy_s(VendorName, DEFAULT_SIZE, "ATI Technologies Inc."); break;
	case 0x10DE: strcpy_s(VendorName, DEFAULT_SIZE, "NVIDIA Corporation"); break;
	case 0x102B: strcpy_s(VendorName, DEFAULT_SIZE, "Matrox Electronic System Ltd."); break;
	case 0x121A: strcpy_s(VendorName, DEFAULT_SIZE, "3dfx Interactive Inc"); break;
	case 0x5333: strcpy_s(VendorName, DEFAULT_SIZE, "S3 Graphics Co. Ltd"); break;
	case 0x8086: strcpy_s(VendorName, DEFAULT_SIZE, "Intel Corporation"); break;
	default:
		sprintf_s(VendorName, DEFAULT_SIZE, "Unknown VendorId: %x", adapDesc.VendorId);
		break;
	}

	// set present params...
	ZeroMemory(&present, sizeof(present));
	present.BufferCount = 1;
	present.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	present.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	present.OutputWindow = hwnd;

	if (vsync)
		present.BufferDesc.RefreshRate.Numerator = 0;
	else
		present.BufferDesc.RefreshRate.Numerator = 60;
	present.BufferDesc.RefreshRate.Denominator = 1;
	
	if (fullscreen)
	{
		present.BufferDesc.Width = (UINT)windowSize.Width;
		present.BufferDesc.Height = (UINT)windowSize.Height;
		present.Windowed = false;	
		present.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

		present.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
		present.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST;
		present.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;
	
		//adjust
		DXGI_MODE_DESC* request = &(present.BufferDesc);
		DXGI_MODE_DESC match;
		ZeroMemory( &match, sizeof( DXGI_MODE_DESC ) );
		hr = DXGIOutput->FindClosestMatchingMode( request, &match, pID3DDevice11 );
		if( SUCCEEDED(hr) ) 
		{
			present.BufferDesc.Format = match.Format;
			present.BufferDesc.Height = match.Height;
			present.BufferDesc.RefreshRate = match.RefreshRate;
			present.BufferDesc.Scaling = match.Scaling;
			present.BufferDesc.ScanlineOrdering = match.ScanlineOrdering;
			present.BufferDesc.Width = match.Width;

			// Adjust screen size accordingly
			ScreenSize.Width = match.Width;
			ScreenSize.Height = match.Height;
		}
	}
	else
	{
		present.BufferDesc.Width = (UINT)windowSize.Width;
		present.BufferDesc.Height = (UINT)windowSize.Height;
		present.Windowed = true;
		present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	//antialias
	if (antialias > 0)
	{
		if(antialias > 16)
			antialias = 16;

		UINT qualityLevels = 0;

		while(antialias > 0)
		{
			if(SUCCEEDED(pID3DDevice11->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,
				(D3DMULTISAMPLE_TYPE)antialias, &qualityLevels)))
			{
				u32 quality = qualityLevels - 1;
				if (VendorID == 0x10DE)  //nvidia
				{
					switch(csaa)
					{
					case ECSAA_8x:	
						if(antialias == D3DMULTISAMPLE_4_SAMPLES && qualityLevels > 8) 
						{	
							quality = 8;
							CSAAApplied = true;
						}
						break;
					case ECSAA_8xQ:		
						if(antialias == D3DMULTISAMPLE_8_SAMPLES && qualityLevels > 8) 
						{	
							quality = 8;
							CSAAApplied = true;
						}
						break;
					case ECSAA_16x:	
						if(antialias == D3DMULTISAMPLE_4_SAMPLES && qualityLevels > 16) 
						{	
							quality = 16;
							CSAAApplied = true;
						}
						break;
					case ECSAA_16xQ:	
						if(antialias == D3DMULTISAMPLE_8_SAMPLES && qualityLevels > 16) 
						{	
							quality = 16;
							CSAAApplied = true;
						}
						break;
					}
				}

				present.SampleDesc.Count	= (D3DMULTISAMPLE_TYPE)antialias;
				present.SampleDesc.Quality = quality;
				break;
			}
			--antialias;
		}
	}
	AntiAliasing = antialias;

	DXGIFactory->MakeWindowAssociation(hwnd, 0);

	//swap chain
	hr = DXGIFactory->CreateSwapChain(pID3DDevice11, &present, &SwapChain);
	if (FAILED(hr))
	{
		hr = DXGIFactory->CreateSwapChain(pID3DDevice11, &present, &SwapChain);
	}
	_ASSERT(SUCCEEDED(hr));

	if (!SwapChain) { _ASSERT(false); return false; }

	DXGIDevice->Release();

	return true;
}

bool CD3D11Driver::beginScene()
{

	return false;
}

bool CD3D11Driver::endScene()
{

	return false;
}

bool CD3D11Driver::clear( bool renderTarget, bool zBuffer, SColor color, float z /*= 1*/ )
{

	return false;
}

bool CD3D11Driver::queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
{

	return false;
}

void CD3D11Driver::setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat )
{

}

void CD3D11Driver::setTexture( u32 stage, ITexture* texture )
{

}

ITexture* CD3D11Driver::getTexture( u32 index ) const
{

	return NULL;
}

void CD3D11Driver::setAmbientLight( SColor color )
{

}

void CD3D11Driver::deleteAllDynamicLights()
{

}

bool CD3D11Driver::addDynamicLight( const SLight& light )
{

	return false;
}

void CD3D11Driver::turnLightOn( u32 lightIndex, bool turnOn )
{

}

void CD3D11Driver::setFog( SFogParam fogParam )
{

}

bool CD3D11Driver::setClipPlane( u32 index, const plane3df& plane )
{

	return false;
}

void CD3D11Driver::enableClipPlane( u32 index, bool enable )
{

}

bool CD3D11Driver::setRenderTarget( ITexture* texture )
{

	return false;
}

void CD3D11Driver::setViewPort( recti area )
{

}

void CD3D11Driver::registerLostReset( ILostResetCallback* callback )
{

}

void CD3D11Driver::setDisplayMode( const dimension2du& size )
{

}

bool CD3D11Driver::changeDriverSettings(bool vsync, u32 antialias)
{
	return true;
}

void CD3D11Driver::draw3DMode( IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType, u32 primCount, SDrawParam drawParam )
{

}

void CD3D11Driver::draw3DMode( IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 startIndex )
{

}

void CD3D11Driver::draw3DModeUP( void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType )
{

}

void CD3D11Driver::draw3DModeUP( void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType )
{

}

void CD3D11Driver::draw2DMode( IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, u32 primCount, SDrawParam drawParam, bool alpha, bool texture, bool alphaChannel )
{

}

void CD3D11Driver::draw2DMode( IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primTpye, u32 primCount, u32 vOffset, u32 vStart, bool alpha, bool texture, bool alphaChannel )
{

}

void CD3D11Driver::draw2DModeUP( void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel )
{

}

void CD3D11Driver::draw2DModeUP( void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel )
{

}

void CD3D11Driver::drawIndexedPrimitive( IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType, u32 primCount, SDrawParam drawParam )
{

}

void CD3D11Driver::drawPrimitive( IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 vStart )
{

}

void CD3D11Driver::drawIndexedPrimitiveUP( void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType )
{

}

void CD3D11Driver::drawPrimitiveUP( void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType )
{

}

void CD3D11Driver::drawDebugInfo( const c8* strMsg )
{

}