#pragma once

#include "base.h"
#include "IVideoDriver.h"
#include "CFPSCounter.h"

class CD3D9ShaderServices;
class CD3D9MaterialRenderServices;

class CD3D9Driver : public IVideoDriver
{
public:
	CD3D9Driver();
	virtual ~CD3D9Driver();

public:
	bool initDriver(HWND hwnd, bool fullscreen, bool vsync, bool multithread, u32 bits, u8 antialias, E_CSAA cass);
	void createVertexDecl();
	void releaseVertexDecl();

public:
	virtual void* getD3DDevice() const { return pID3DDevice; }

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, SColor color, float z = 1);

	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat );
	virtual const matrix4& getTransform( E_TRANSFORMATION_STATE state ) const { return CurrentDeviceState.matrices[state]; }

	virtual void setMaterial(const SMaterial& material);
	virtual const SMaterial& getMaterial() const { return Material; }

	virtual void setTexture(u32 stage, ITexture* texture);
	virtual ITexture* getTexture(u32 index) const;

	//scene states, light & fog
	virtual SColor getAmbientLight() const { return SceneState.AmbientLightColor; }
	virtual void setAmbientLight( SColor color );
	virtual void deleteAllDynamicLights();
	virtual bool addDynamicLight( const SLight& light );
	virtual u32 getDynamicLightCount() const  { return SceneState.LightCount; }
	virtual const SLight& getDynamicLight(u32 index) const { return SceneState.Lights[index]; }
	virtual void turnLightOn(u32 lightIndex, bool turnOn);

	virtual void setFog(SFogParam fogParam);
	virtual SFogParam getFog() const { return SceneState.FogParam; }

	virtual bool setClipPlane( u32 index, const plane3df& plane );
	virtual void enableClipPlane( u32 index, bool enable );

	virtual bool setRenderTarget( ITexture* texture );

	virtual void setViewPort( recti area );
	virtual const recti& getViewPort() const { return Viewport; }

	virtual void registerLostReset( ILostResetCallback* callback );

	virtual void setDisplayMode(const dimension2du& size);

	virtual bool changeDriverSettings(bool vsync, u32 antialias);

	virtual void draw3DMode(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType,
		u32 primCount, SDrawParam drawParam);

	virtual void draw3DMode(IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 startIndex);

	virtual void draw3DModeUP(void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType);
	virtual void draw3DModeUP(void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType);
	
	virtual void draw2DMode(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType,
		u32 primCount, SDrawParam drawParam,
		bool alpha, bool texture, bool alphaChannel);
	virtual void draw2DMode(IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primTpye, u32 primCount, u32 vOffset, u32 vStart,
		bool alpha, bool texture, bool alphaChannel);

	virtual void draw2DModeUP(void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel);
	virtual void draw2DModeUP(void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel);

	virtual void drawIndexedPrimitive(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType,
		u32 primCount, SDrawParam drawParam);	
	virtual void drawPrimitive(IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 vStart);	
	virtual void drawIndexedPrimitiveUP(void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType);	
	virtual void drawPrimitiveUP(void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType);

	virtual void drawDebugInfo(const c8* strMsg);

	virtual IMaterialRenderServices* getMaterialRenderServices() const { return (IMaterialRenderServices*)MaterialRenderServices; }
	virtual IShaderServices*	getShaderServices() const { return (IShaderServices*)ShaderServices; }

private:
	bool reset();
	void setVertexDeclaration(E_VERTEX_TYPE type);
	IDirect3DVertexDeclaration9* getD3DVertexDeclaration(E_VERTEX_TYPE type) { return VertexDeclarations[type]; }
	void setRenderState3DMode();
	void setRenderState2DMode( bool alpha, bool texture, bool alphaChannel );
	f32 getFPS() const { return FPSCounter.getFPS(); }

 	typedef IDirect3D9* (__stdcall *D3DCREATETYPE)(UINT);
 	typedef HRESULT (__stdcall *D3DCREATETYPEEX)(UINT, IDirect3D9Ex**);

	void resetSceneStateCache();
	struct SSceneStateCache			//SceneState的缓存
	{
		DWORD		Ambient;
		DWORD		FogColor;
		DWORD		FogTableMode;
		DWORD		FogVertexMode;
		DWORD		FogStart;
		DWORD		FogEnd;
		DWORD		FogDensity;
		DWORD		RangeFogEnable;
	};
	SSceneStateCache	SSCache;

public:
	HMODULE		HLib;
	IDirect3D9*		pID3D;
	IDirect3DDevice9*	pID3DDevice;

	D3DPRESENT_PARAMETERS		present;
	D3DCAPS9		Caps;

	//DepthStencil, backbuffer
	IDirect3DSurface9*	DepthStencilSurface;
	dimension2du	DepthStencilSize;
	IDirect3DSurface9*		BackBuffer;
	D3DFORMAT		BackBufferFormat;

	//rendertarget
	IDirect3DSurface9*		PrevRenderTarget;			//backbuffer RT0
	dimension2du		CurrentRenderTargetSize;

	//device settings
	HWND		HWnd;
	UINT	Adapter;
	D3DDEVTYPE	DevType;
	bool		VSync;
	bool		FullScreen;
	u8		AntiAliasing;
	u8		Quality;
	u32		MaxTextureUnits;
	u32		MaxUserClipPlanes;
	c8		VendorName[DEFAULT_SIZE];
	u32		VendorID;
	bool		AlphaToCoverageSupport;		//MSAA
	bool		CSAAApplied;		//CSAA

public:
		struct SDeviceState				//除去material以外的state，material设置时已经做了cache
		{
			void reset()
			{
				VertexType = (E_VERTEX_TYPE)-1;
				vBuffer= vBuffer2 = NULL;
				iBuffer = NULL;
				vOffset = vOffset2 = 0;
				memset(matrices, 0, sizeof(matrices));

				memset(enableclips, 0, sizeof(enableclips));
				memset(clipplanes, 0, sizeof(clipplanes));
			}

			E_VERTEX_TYPE	VertexType;
			IVertexBuffer*	vBuffer;
			u32		vOffset;
			IVertexBuffer*	vBuffer2;
			u32		vOffset2;
			IIndexBuffer*		iBuffer;
			matrix4		matrices[ETS_COUNT];

			plane3df		clipplanes[5];
			u32		enableclips[5];
		};

private:
	SMaterial	Material, LastMaterial;
	
	recti		Viewport;
	dimension2du	ScreenSize;

	bool DeviceLost;

	typedef std::list<ILostResetCallback*, qzone_allocator<ILostResetCallback*>> T_LostResetList;
	T_LostResetList	LostResetList;

	SSceneState		SceneState;

	// device state cache
	SDeviceState	CurrentDeviceState;
	IMaterialRenderer*		MaterialRenderer;
	CD3D9ShaderServices*	ShaderServices;
	CD3D9MaterialRenderServices*		MaterialRenderServices;

	E_RENDER_MODE		CurrentRenderMode;
	bool		ResetRenderStates;

	char		DeviceDescription[512];
	char		DeviceName[32];

	CFPSCounter		FPSCounter;
	u32		PrimitivesDrawn;
	u32		DrawCall;

	//2D
	SMaterial	InitMaterial2D;

	IDirect3DVertexDeclaration9*		VertexDeclarations[EVT_COUNT];

	c8		DebugMsg[512];
};