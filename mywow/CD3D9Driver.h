#pragma once

#include "base.h"
#include "IVideoDriver.h"

class CD3D9SceneStateServices;
class CD3D9ShaderServices;
class CD3D9MaterialRenderServices;

class CD3D9Driver : public IVideoDriver
{
public:
	CD3D9Driver();
	virtual ~CD3D9Driver();

public:
	bool initDriver(HWND hwnd, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread);

public:
	//base
	virtual u32 getAdapterCount() const { return AdapterCount; }
	virtual void* getD3DDevice() const { return pID3DDevice; }

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color);

	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat );
	virtual const matrix4& getTransform( E_TRANSFORMATION_STATE state ) const { return CurrentDeviceState.matrices[state]; }

	virtual void setMaterial(const SMaterial& material);
	virtual const SMaterial& getMaterial() const { return Material; }
	virtual SOverrideMaterial& getOverrideMaterial() { return OverrideMaterial; }

	virtual void setTexture(u32 stage, ITexture* texture);
	virtual ITexture* getTexture(u32 index) const;

	virtual bool setRenderTarget( IRenderTarget* texture );

	virtual void setViewPort( recti area );
	virtual const recti& getViewPort() const { return Viewport; }

	virtual void registerLostReset( ILostResetCallback* callback ){ LostResetList.push_back(callback); }
	virtual void setDisplayMode(const dimension2du& size);
	virtual dimension2du getDisplayMode() const;
	virtual bool setDriverSetting(const SDriverSetting& setting);
	virtual const SDriverSetting& getDriverSetting() const { return DriverSetting; }

	virtual ISceneStateServices* getSceneStateServices() const { return (ISceneStateServices*)SceneStateServices; }
	virtual IMaterialRenderServices* getMaterialRenderServices() const { return (IMaterialRenderServices*)MaterialRenderServices; }
	virtual IShaderServices*	getShaderServices() const { return (IShaderServices*)ShaderServices; }

	//draw
	virtual void draw3DMode( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam);

	virtual void draw2DMode( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam,
		bool alpha, bool alphaChannel, E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend);

	virtual void drawIndexedPrimitive( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam);
	virtual void drawPrimitive( const SBufferParam& bufferParam,
		E_PRIMITIVE_TYPE primType,
		u32 primCount,
		const SDrawParam& drawParam);
	virtual void drawDebugInfo(const c8* strMsg);

private:
	bool reset();
	void createVertexDecl();
	void releaseVertexDecl();
	void setVertexDeclaration(E_VERTEX_TYPE type);
	IDirect3DVertexDeclaration9* getD3DVertexDeclaration(E_VERTEX_TYPE type);
	void setRenderState3DMode();
	void setRenderState2DMode( bool alpha, bool alphaChannel, E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend );

public:
	HMODULE		HLib;
	IDirect3D9*		pID3D;
	IDirect3DDevice9*	pID3DDevice;

	D3DPRESENT_PARAMETERS		present;
	D3DCAPS9		Caps;

	//DepthStencil, backbuffer
	IDirect3DSurface9*		DefaultDepthBuffer;
	IDirect3DSurface9*		DefaultBackBuffer;
	D3DFORMAT		BackBufferFormat;

	//rendertarget
	IRenderTarget*		CurrentRenderTarget;

	//device settings
	HWND		HWnd;
	D3DDEVTYPE	DevType;

	CAdapterInfo	AdapterInfo;
	SDriverSetting	DriverSetting;

	bool		AlphaToCoverageSupport;

private:
		struct SDeviceState				//除去material以外的state，material设置时已经做了cache
		{
			void reset()
			{
				vType = (E_VERTEX_TYPE)-1;
				vBuffer0= vBuffer1 = vBuffer2 = vBuffer3 = NULL;
				iBuffer = NULL;
				vOffset0 = vOffset1 = vOffset2 = vOffset3 = 0;
				memset(matrices, 0, sizeof(matrices));
			}

			E_VERTEX_TYPE 	vType;
			IVertexBuffer*	vBuffer0;
			IVertexBuffer	*vBuffer1;
			IVertexBuffer*	vBuffer2;
			IVertexBuffer*	vBuffer3;

			u32		vOffset0;
			u32		vOffset1;
			u32		vOffset2;
			u32		vOffset3;

			IIndexBuffer*		iBuffer;
			matrix4		matrices[ETS_COUNT];
		};

private:
	SMaterial	Material, LastMaterial;
	SOverrideMaterial		OverrideMaterial;
	
	recti		Viewport;
	dimension2du	ScreenSize;

	bool DeviceLost;

	typedef std::list<ILostResetCallback*, qzone_allocator<ILostResetCallback*>> T_LostResetList;
	T_LostResetList	LostResetList;

	// device state cache
	SDeviceState	CurrentDeviceState;
	IMaterialRenderer*		MaterialRenderer;
	CD3D9SceneStateServices*	SceneStateServices;
	CD3D9ShaderServices*	ShaderServices;
	CD3D9MaterialRenderServices*		MaterialRenderServices;

	E_RENDER_MODE		CurrentRenderMode;
	bool		ResetRenderStates;

	u32		AdapterCount;

	u32		PrimitivesDrawn;
	u32		DrawCall;

	//2D
	SMaterial	InitMaterial2D;
	SOverrideMaterial		InitOverrideMaterial2D;

	IDirect3DVertexDeclaration9*		VertexDeclarations[EVT_COUNT];

	c8		DebugMsg[512];
};

