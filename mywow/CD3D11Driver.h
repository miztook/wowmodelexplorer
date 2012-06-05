#pragma once

#include "base.h"
#include "IVideoDriver.h"
#include "CFPSCounter.h"

class CD3D11Driver : public IVideoDriver
{
public:
	CD3D11Driver();
	virtual ~CD3D11Driver();

public:
	bool initDriver(HWND hwnd, bool fullscreen, bool vsync, bool multithread, u8 antialias, E_CSAA csaa);

public:
	virtual void* getD3DDevice() const { return SwapChain; }

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, SColor color, float z = 1);

	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat );
	virtual const matrix4& getTransform( E_TRANSFORMATION_STATE state ) const { return CurrentDeviceState.matrices[state]; }

	virtual void setMaterial(const SMaterial& material) { Material = material; }
	virtual const SMaterial& getMaterial() const { return Material; }

	virtual void setTexture(u32 stage, ITexture* texture);
	virtual ITexture* getTexture(u32 index) const;

	SColor getAmbientLight() const { return SceneState.AmbientLightColor; }
	virtual void setAmbientLight( SColor color );
	virtual void deleteAllDynamicLights();
	virtual bool addDynamicLight( const SLight& light );
	u32 getDynamicLightCount() const  { return SceneState.LightCount; }
	const SLight& getDynamicLight(u32 index) const { return SceneState.Lights[index]; }
	virtual void turnLightOn(u32 lightIndex, bool turnOn);

	virtual void setFog(SFogParam fogParam);
	SFogParam getFog() const { return SceneState.FogParam; }

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

	virtual IMaterialRenderServices* getMaterialRenderServices() const { return MaterialRenderServices; }
	virtual IShaderServices*	getShaderServices() const { return ShaderServices; }

public:
	HMODULE	m_hD3DLib;

	// DXGI objects
	DXGI_SWAP_CHAIN_DESC present;
	IDXGISwapChain* SwapChain;
	IDXGIAdapter* Adapter;
	IDXGIOutput* DXGIOutput;
	IDXGIFactory* DXGIFactory;

	//D3D 11 Device Objects
	D3D_FEATURE_LEVEL		FeatureLevel;
	ID3D11Device*		pID3DDevice11;
	ID3D11DeviceContext*	ImmediateContext;

	u8		AntiAliasing;
	u32		MaxTextureUnits;
	u32		MaxUserClipPlanes;
	bool		AlphaToCoverageSupport;		//MSAA
	bool		CSAAApplied;		//CSAA

	c8		VendorName[DEFAULT_SIZE];
	u32		VendorID;

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

	SSceneState		SceneState;

	// device state cache
	SDeviceState	CurrentDeviceState;

	IShaderServices*	ShaderServices;
	IMaterialRenderServices*		MaterialRenderServices;

	E_RENDER_MODE		CurrentRenderMode;
	bool		ResetRenderStates;

	c16		DeviceDescription[512];
	c16		DeviceName[32];
};