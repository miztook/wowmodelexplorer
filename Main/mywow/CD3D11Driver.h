#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "IVideoDriver.h"
#include <list>

class CD3D11SceneStateServices;
class CD3D11ShaderServices;
class CD3D11MaterialRenderServices;
class CD3D11VertexDeclaration;
class IVertexShader;
struct SWindowInfo;

class CD3D11Driver : public IVideoDriver
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11Driver);

public:
	CD3D11Driver();
	virtual ~CD3D11Driver();

public:
	bool initDriver(const SWindowInfo& wndInfo, uint32_t adapter, bool fullscreen, bool vsync, uint8_t antialias, bool multithread);

public:
	virtual E_DRIVER_TYPE getDriverType() const { return EDT_DIRECT3D11; }
	virtual uint32_t getAdapterCount() const { return AdapterCount; }

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color);

	virtual bool checkValid() { return true; }
	virtual bool setRenderTarget( IRenderTarget* texture );

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat );
	virtual void setViewPort( recti area );
	virtual void setDisplayMode(const dimension2du& size);
	virtual bool setDriverSetting(const SDriverSetting& setting);

	virtual void drawDebugInfo(const char* strMsg);

	//helper functions
	virtual void helper_render(CMeshRenderer* meshRenderer, const SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_render(CTerrainRenderer* terrainRenderer, const SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_render(CTransluscentRenderer* transluscentRenderer, const SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_render(CWmoRenderer* wmoRenderer, const SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_render(CAlphaTestMeshRenderer* meshRenderer, const SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_render(CAlphaTestWmoRenderer* wmoRenderer, const SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_renderAllBatches(CParticleRenderer* particleRenderer, const SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_renderAllBatches(CRibbonRenderer* ribbonRenderer, const SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_renderAllBatches(CMeshDecalRenderer* meshDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_renderAllBatches(CTransluscentDecalRenderer* transDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_renderAllBatches(CAlphaTestDecalRenderer* meshDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam);

public:
	bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	void setTexture(uint32_t stage, ITexture* texture);
	ITexture* getTexture(uint32_t index) const;

	void setTransform(const matrix4& matView, const matrix4& matProjection);

	void setTransform_Material_Textures(const matrix4& matWorld,
		const SMaterial& material,
		ITexture* const textures[],
		uint32_t numTextures);

	void setTransform_Material_Textures(const matrix4& matWorld,
		const matrix4& matView, 
		const matrix4& matProjection,
		const SMaterial& material,
		ITexture* const textures[],
		uint32_t numTextures);

	//draw
	void draw3DMode( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount, 
		const SDrawParam& drawParam);

	void draw2DMode( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount, 
		const SDrawParam& drawParam,
		const S2DBlendParam& blendParam,
		bool zTest = false);

private:
	void drawIndexedPrimitive( const SBufferParam& bufferParam, IVertexShader* vs, 
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount, 
		const SDrawParam& drawParam);
	void drawPrimitive( const SBufferParam& bufferParam, IVertexShader* vs, 
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount,
		const SDrawParam& drawParam);

	void recreateDepthStencilView(dimension2du size, ECOLOR_FORMAT depthFmt, uint32_t antialias, uint32_t quality);
	bool reset(bool recreateSwapChain);
	void createVertexDecl();
	void releaseVertexDecl();
	 CD3D11VertexDeclaration* getVertexDeclaration(E_VERTEX_TYPE type) const { return VertexDeclarations[type]; }
	void setVertexDeclaration(E_VERTEX_TYPE type, IVertexShader* vs);
	void setRenderState3DMode(E_VERTEX_TYPE vType);
	void setRenderState2DMode(E_VERTEX_TYPE vType, const S2DBlendParam& blendParam );
	void logCaps();

public:
	HMODULE		HLibDXGI;
	HMODULE		HLibD3D;
	// DXGI objects
	DXGI_SWAP_CHAIN_DESC Present;
	IDXGISwapChain* SwapChain;
	IDXGIAdapter* Adapter;
	IDXGIOutput* DXGIOutput;
	IDXGIFactory* DXGIFactory;

	//D3D 11 Device Objects
	D3D_FEATURE_LEVEL		FeatureLevel;
	ID3D11Device*		pID3DDevice11;
	ID3D11DeviceContext*	ImmediateContext;

	// Back and depth buffers
	ID3D11RenderTargetView* DefaultBackBuffer;
	ID3D11Texture2D*		DefaultDepthTexture;
	ID3D11DepthStencilView* DefaultDepthBuffer;
	DXGI_FORMAT		BackBufferFormat;
	DXGI_FORMAT		DepthStencilFormat;	
	DXGI_FORMAT		DepthTextureFormat;
	DXGI_FORMAT		DepthSRFormat;

	//rendertarget
	IRenderTarget*		CurrentRenderTarget;

	//device settings
	HWND		HWnd;
	D3D_DRIVER_TYPE	DevType;

	CAdapterInfo	AdapterInfo;

private:
	struct SDeviceState				//除去material以外的state，material设置时已经做了cache
	{
		void reset()
		{
			vType = (E_VERTEX_TYPE)-1;
			vBuffer0= vBuffer1 = nullptr;
			iBuffer = nullptr;
			vOffset0 = vOffset1 = 0;
		}

		E_VERTEX_TYPE 	vType;
		CVertexBuffer*	vBuffer0;
		CVertexBuffer	*vBuffer1;

		uint32_t		vOffset0;
		uint32_t		vOffset1;

		CIndexBuffer*		iBuffer;
	};

private:
	uint32_t		AdapterCount;
	uint32_t		PrimitivesDrawn;
	uint32_t		DrawCall;

	// device state cache
	SDeviceState	CurrentDeviceState;
	IMaterialRenderer*		MaterialRenderer;
	CD3D11SceneStateServices*	D3D11SceneStateServices;
	CD3D11ShaderServices*	D3D11ShaderServices;
	CD3D11MaterialRenderServices*		D3D11MaterialRenderServices;

	E_RENDER_MODE		CurrentRenderMode;

	CD3D11VertexDeclaration* VertexDeclarations[EVT_COUNT];

	char		DebugMsg[512];	
	
	bool		ResetRenderStates;

	friend class CD3D11MaterialRenderServices;
};

#endif

