#pragma once

#include "core.h"
#include "SColor.h"
#include "SMaterial.h"
#include "SLight.h"
#include <list>

class IMaterialRenderer;
class IVertexBuffer;
class IIndexBuffer;
class IMaterialRenderServices;
class IShaderServices;

enum E_RENDER_MODE
{
	ERM_NONE = 0,
	ERM_2D,
	ERM_3D,
	ERM_STENCIL_FILL,
	ERM_SHADOW_VOLUME_ZFAIL,
	ERM_SHADOW_VOLUME_ZPASS,

	ERM_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

struct SFogParam
{
	SFogParam()
		: FogStart(50.0f), FogEnd(100.0f), FogDensity(0.01f), FogType(EFT_FOG_LINEAR), PixelFog(false), RangeFog(false)
	{ }

	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;
	SColor	FogColor;
	E_FOG_TYPE FogType;
	bool PixelFog;
	bool RangeFog;
};

class IVideoDriver
{
public:
	virtual ~IVideoDriver() { }

public:
	virtual void* getD3DDevice() const = 0;

	virtual bool beginScene() = 0;
	virtual bool endScene() = 0;
	virtual bool clear(bool renderTarget, bool zBuffer, SColor color, float z = 1) = 0;

	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const =0;

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat ) = 0;
	virtual const matrix4& getTransform( E_TRANSFORMATION_STATE state ) const = 0;

	virtual void setMaterial(const SMaterial& material) = 0;
	virtual const SMaterial& getMaterial() const = 0;

	virtual void setTexture(u32 stage, ITexture* texture) = 0;
	virtual ITexture* getTexture(u32 index) const= 0;

	virtual SColor getAmbientLight() const = 0;
	virtual void setAmbientLight( SColor color ) = 0;
	virtual void deleteAllDynamicLights() = 0;
	virtual bool addDynamicLight( const SLight& light )  = 0;
	virtual u32 getDynamicLightCount() const = 0;
	virtual const SLight& getDynamicLight(u32 index) const  = 0;
	virtual void turnLightOn(u32 lightIndex, bool turnOn) = 0;

	virtual void setFog(SFogParam fogParam) = 0;
	virtual SFogParam getFog() const = 0;

	virtual bool setClipPlane( u32 index, const plane3df& plane ) = 0;
	virtual void enableClipPlane( u32 index, bool enable ) = 0;

	virtual bool setRenderTarget( ITexture* texture ) = 0;

	virtual void setViewPort( recti area ) = 0;
	virtual const recti& getViewPort() const = 0;

	virtual void registerLostReset( ILostResetCallback* callback ) = 0;
	virtual void setDisplayMode(const dimension2du& size) = 0;
	virtual bool changeDriverSettings(bool vsync, u32 antialias) = 0;

	virtual void draw3DMode(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType,
		u32 primCount, SDrawParam drawParam) = 0;
	virtual void draw3DMode(IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 startIndex) = 0;

	virtual void draw3DModeUP(void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType) = 0;
	virtual void draw3DModeUP(void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType) = 0;
	
	virtual void draw2DMode(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType,
		u32 primCount, SDrawParam drawParam,
		bool alpha, bool texture, bool alphaChannel) = 0;
	virtual void draw2DMode(IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primTpye, u32 primCount, u32 vOffset, u32 vStart,
		bool alpha, bool texture, bool alphaChannel) = 0;
	
	virtual void draw2DModeUP(void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel) = 0;
	virtual void draw2DModeUP(void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType, bool alpha, bool texture, bool alphaChannel) = 0;

	virtual void drawIndexedPrimitive(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, IVertexBuffer* vbuffer2, E_PRIMITIVE_TYPE primType,
		u32 primCount, SDrawParam drawParam) = 0;
	virtual void drawPrimitive(IVertexBuffer* vbuffer, E_PRIMITIVE_TYPE primType, u32 primCount, u32 voffset, u32 vStart) = 0;
	virtual void drawIndexedPrimitiveUP(void* vertices, E_VERTEX_TYPE vType, void* indices, E_INDEX_TYPE iType, u32 iCount, u32 vStart, u32 vCount, E_PRIMITIVE_TYPE primType) = 0;
	virtual void drawPrimitiveUP(void* vertices, E_VERTEX_TYPE vType, u32 vCount, E_PRIMITIVE_TYPE primType) = 0;

	virtual void drawDebugInfo(const c8* strMsg) = 0;

	virtual IMaterialRenderServices* getMaterialRenderServices() const = 0;
	virtual IShaderServices*	getShaderServices() const = 0;

protected:
	struct SSceneState
	{
		SSceneState() { resetLights(); }
		
		void resetLights()
		{
			LightCount = 0;
			memset(LightsOn, 0, sizeof(LightsOn)); 
		}

		SLight		Lights[8];
		bool		LightsOn[8];
		u32			LightCount;
		SColor		AmbientLightColor;
		SFogParam	FogParam;
	};

public:


};
