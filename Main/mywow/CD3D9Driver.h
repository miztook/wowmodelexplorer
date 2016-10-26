#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IVideoDriver.h"

class CD3D9SceneStateServices;
class CD3D9ShaderServices;
class CD3D9MaterialRenderServices;
class CD3D9VertexDeclaration;
struct SWindowInfo;

class CD3D9Driver : public IVideoDriver
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9Driver);

public:
	CD3D9Driver();
	virtual ~CD3D9Driver();

public:
	bool initDriver(const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread);

public:
	virtual E_DRIVER_TYPE getDriverType() const { return EDT_DIRECT3D9; }
	virtual u32 getAdapterCount() const { return AdapterCount; }

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color);

	virtual SOverrideMaterial& getOverrideMaterial() { return OverrideMaterial; }

	virtual bool checkValid();
	virtual bool setRenderTarget( IRenderTarget* texture );

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat );
	virtual void setViewPort( recti area );
	virtual void setDisplayMode(const dimension2du& size);
	virtual bool setDriverSetting(const SDriverSetting& setting);

	virtual void drawDebugInfo(const c8* strMsg);

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

	void setTexture(u32 stage, ITexture* texture);
	ITexture* getTexture(u32 index) const;
	void registerLostReset( ILostResetCallback* callback );
	void removeLostReset( ILostResetCallback* callback );

	//draw
	void draw3DMode( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam);

	void draw2DMode( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam,
		const S2DBlendParam& blendParam,
		bool zTest = false);

	void setTransform(const matrix4& matView, const matrix4& matProjection);

	void setTransform_Material_Textures(const matrix4& matWorld,
		const SMaterial& material,
		ITexture* const textures[],
		u32 numTextures);

	void setTransform_Material_Textures(const matrix4& matWorld,
		const matrix4& matView, 
		const matrix4& matProjection,
		const SMaterial& material,
		ITexture* const textures[],
		u32 numTextures);

private:
	void drawIndexedPrimitive( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam);
	void drawPrimitive( const SBufferParam& bufferParam,
		E_PRIMITIVE_TYPE primType,
		u32 primCount,
		const SDrawParam& drawParam);

	bool reset();
	void createVertexDecl();
	void releaseVertexDecl();
	void setVertexDeclaration(E_VERTEX_TYPE type);
	 CD3D9VertexDeclaration* getVertexDeclaration(E_VERTEX_TYPE type) const { return VertexDeclarations[type]; }
	void setRenderState3DMode(E_VERTEX_TYPE vType);
	void setRenderState2DMode(E_VERTEX_TYPE vType, const S2DBlendParam& blendParam );
	void logCaps();

public:
	HMODULE		HLib;
	IDirect3D9*		pID3D;
	IDirect3DDevice9*	pID3DDevice;

	D3DPRESENT_PARAMETERS		Present;
	D3DCAPS9		Caps;

	//DepthStencil, backbuffer
	IDirect3DSurface9*		DefaultDepthBuffer;
	IDirect3DSurface9*		DefaultBackBuffer;
	//rendertarget
	IRenderTarget*		CurrentRenderTarget;
	D3DFORMAT		BackBufferFormat;

	//device settings
	HWND		HWnd;
	D3DDEVTYPE		DevType;

	CAdapterInfo	AdapterInfo;

private:
	struct SDeviceState				//除去material以外的state，material设置时已经做了cache
	{
		void reset()
		{
			vType = (E_VERTEX_TYPE)-1;
			vBuffer0= vBuffer1 = NULL_PTR;
			iBuffer = NULL_PTR;
			vOffset0 = vOffset1 = 0;
		}

		E_VERTEX_TYPE 	vType;
		IVertexBuffer*	vBuffer0;
		IVertexBuffer	*vBuffer1;

		u32		vOffset0;
		u32		vOffset1;

		IIndexBuffer*		iBuffer;
	};

private:
	E_RENDER_MODE		CurrentRenderMode;

	// device state cache
	SDeviceState	CurrentDeviceState;
	IMaterialRenderer*		MaterialRenderer;
	CD3D9SceneStateServices*	D3D9SceneStateServices;
	CD3D9ShaderServices*	D3D9ShaderServices;
	CD3D9MaterialRenderServices*		D3D9MaterialRenderServices;

	CD3D9VertexDeclaration*		VertexDeclarations[EVT_COUNT];

	u32		AdapterCount;
	u32		PrimitivesDrawn;
	u32		DrawCall;

	typedef std::list<ILostResetCallback*, qzone_allocator<ILostResetCallback*> > T_LostResetList;
	T_LostResetList	LostResetList;

	c8		DebugMsg[512];

	bool		DeviceLost;
	bool		ResetRenderStates;
	bool		AlphaToCoverageSupport;

	friend class CD3D9MaterialRenderServices;
};

#endif
