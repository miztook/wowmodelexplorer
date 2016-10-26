#pragma once

#include "core.h"
#include "SMaterial.h"
#include <algorithm>
#include <list>
#include "ILostResetCallback.h"

struct SBufferParam;
class IRenderTarget;
class IMaterialRenderer;
class IVertexBuffer;
class IIndexBuffer;
class ISceneStateServices;
class IMaterialRenderServices;
class IShaderServices;

class CMeshRenderer;
class CParticleRenderer;
class CRibbonRenderer;
class CTerrainRenderer;
class CTransluscentRenderer;
class CWmoRenderer;
class CMeshDecalRenderer;
class CTransluscentDecalRenderer;
class CAlphaTestWmoRenderer;
class CAlphaTestMeshRenderer;
class CAlphaTestDecalRenderer;
struct SRenderUnit;
class ICamera;

enum E_RENDER_MODE : int32_t
{
	ERM_NONE = 0,
	ERM_2D,
	ERM_3D,
	ERM_STENCIL_FILL,
};

struct SDriverSetting 
{
	bool vsync;	
	bool fullscreen;
	u8	antialias;
	u8	quality;
};

struct SDisplayMode
{
	u16 width;
	u16 height;
	u32 refreshrate;	
	
	bool operator==(const SDisplayMode& mode) const
	{
		return width == mode.width &&
			height == mode.height &&
			refreshrate == mode.refreshrate;
	}
};

class CAdapterInfo
{
private:
	DISALLOW_COPY_AND_ASSIGN(CAdapterInfo);

public:
	CAdapterInfo() : index(0), vendorID(0)
	{
		::memset(description, 0, sizeof(description));
		::memset(name, 0, sizeof(name));
		::memset(vendorName, 0, DEFAULT_SIZE);
	}

public:
	u32		index;
	u32		vendorID;
	c8		description[512];
	c8		name[DEFAULT_SIZE];
	c8		vendorName[DEFAULT_SIZE];
	
	typedef std::list<SDisplayMode, qzone_allocator<SDisplayMode>  >	T_DisplayModeList;
	T_DisplayModeList displayModes;

	void addDisplayMode(const SDisplayMode& mode)
	{
		if(displayModes.end() == std::find(displayModes.begin(), displayModes.end(), mode))
		{
			displayModes.push_back(mode);
		}
	}

	SDisplayMode getCloseMatchDisplayMode(u32 width, u32 height) const
	{
		SDisplayMode ret;
		//select first
		T_DisplayModeList::const_iterator itr = displayModes.begin();
		u32 diff = abs(((s32)itr->width - (s32)width)) + abs(((s32)itr->height - (s32)height));
		ret = (*itr);

		++itr;
		for (; itr != displayModes.end(); ++itr)
		{
			u32 d = abs(((s32)itr->width - (s32)width)) + abs(((s32)itr->height - (s32)height));
			if ( d < diff)
			{
				diff = d;
				ret = (*itr);
			}
		}

		return ret;
	}
};

//标准高度，以像素为单位的2D元素都是相对于这个值
#define STANDARD_DISPLAY_HEIGHT		600.0f;

class IVideoDriver
{
public:
	IVideoDriver()
	{
		ShaderServices = NULL_PTR;
		MaterialRenderServices = NULL_PTR;
		SceneStateServices = NULL_PTR;
		memset(&DriverSetting, 0, sizeof(DriverSetting));
	}
	virtual ~IVideoDriver() {}

public:
	SOverrideMaterial& getOverrideMaterial() { return OverrideMaterial; }
	const matrix4& getTransform( E_TRANSFORMATION_STATE state ) const { return Matrices[state]; }
	const recti& getViewPort() const { return Viewport; }
	const dimension2du& getDisplayMode() const { return ScreenSize; }
	const SDriverSetting& getDriverSetting() const { return DriverSetting; }

	void setMaterial(const SMaterial& material) { Material = material; }
	const SMaterial& getMaterial() const { return Material; }

	void getWVPMatrix(matrix4& mat) const { mat = WVP; }
	void getWVMatrix(matrix4& mat) const { mat = WV; }
	void getWMatrix(matrix4& mat) const { mat = getTransform(ETS_WORLD); }	

	ISceneStateServices* getSceneStateServices() const { return SceneStateServices; }
	IMaterialRenderServices* getMaterialRenderServices() const { return MaterialRenderServices; }
	IShaderServices*	getShaderServices() const { return ShaderServices; }

	//scale 
	f32 getDisplay2DScale() const { return (f32)ScreenSize.Height / STANDARD_DISPLAY_HEIGHT; }

public:
	virtual E_DRIVER_TYPE getDriverType() const = 0;
	virtual u32 getAdapterCount() const = 0;

	virtual bool beginScene() = 0;
	virtual bool endScene() = 0;
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color) = 0;
	
	virtual bool checkValid() = 0;
	virtual bool setRenderTarget( IRenderTarget* texture ) = 0;

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat ) = 0;
	virtual void setViewPort( recti area ) = 0;
	virtual void setDisplayMode(const dimension2du& size) = 0;
	virtual bool setDriverSetting(const SDriverSetting& setting) = 0;

	virtual void drawDebugInfo(const c8* strMsg) = 0;

	//helper functions
	virtual void helper_render(CMeshRenderer* meshRenderer, const SRenderUnit*& currentUnit,  ICamera* cam) = 0;
	virtual void helper_render(CTerrainRenderer* terrainRenderer, const SRenderUnit*& currentUnit,  ICamera* cam) = 0;
	virtual void helper_render(CTransluscentRenderer* transluscentRenderer, const SRenderUnit*& currentUnit,  ICamera* cam) = 0;
	virtual void helper_render(CWmoRenderer* wmoRenderer, const SRenderUnit*& currentUnit, ICamera* cam) = 0;
	virtual void helper_render(CAlphaTestMeshRenderer* meshRenderer, const SRenderUnit*& currentUnit, ICamera* cam) = 0;
	virtual void helper_render(CAlphaTestWmoRenderer* wmoRenderer, const SRenderUnit*& currentUnit, ICamera* cam) = 0;
	virtual void helper_renderAllBatches(CParticleRenderer* particleRenderer, const SRenderUnit*& currentUnit,  ICamera* cam) = 0;
	virtual void helper_renderAllBatches(CRibbonRenderer* ribbonRenderer, const SRenderUnit*& currentUnit,  ICamera* cam) = 0;
	virtual void helper_renderAllBatches(CMeshDecalRenderer* meshDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam) = 0;
	virtual void helper_renderAllBatches(CTransluscentDecalRenderer* transDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam) = 0;
	virtual void helper_renderAllBatches(CAlphaTestDecalRenderer* meshDecalRenderer, const SRenderUnit*& currentUnit, ICamera* cam) = 0;

#ifdef FULL_INTERFACE

	virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const =0;
	virtual void setTexture(u32 stage, ITexture* texture) = 0;
	virtual ITexture* getTexture(u32 index) const= 0;
	virtual void registerLostReset( ILostResetCallback* callback ) = 0;
	virtual void removeLostReset( ILostResetCallback* callback ) = 0;

	virtual void draw3DMode(const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam) = 0;

	virtual void draw2DMode(const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam,
		const S2DBlendParam& blendParam,
		bool zTest = false) = 0;

	virtual void setTransform_Material_Textures(const matrix4& matWorld,
		const SMaterial& material,
		ITexture* const textures[],
		u32 numTextures) = 0;

	virtual void setTransform_Material_Textures(const matrix4& matWorld,
		const matrix4& matView, 
		const matrix4& matProjection,
		const SMaterial& material,
		ITexture* const textures[],
		u32 numTextures) = 0;

#endif
	
protected:
	SMaterial	Material;
	SMaterial LastMaterial;
	SOverrideMaterial		OverrideMaterial;
	//2D
	SMaterial	InitMaterial2D;
	SMaterial	InitMaterial2DZTest;
	SMaterial  InitMaterial2DScissorTest;
	SMaterial	InitMaterial2DScissorZTest;
	SOverrideMaterial		InitOverrideMaterial2D;

	matrix4 WVP;
	matrix4		WV;
	matrix4		Matrices[ETS_COUNT];
	
	recti		Viewport;
	dimension2du	ScreenSize;		
	SDriverSetting	DriverSetting;

	ISceneStateServices*	SceneStateServices;
	IShaderServices*	ShaderServices;
	IMaterialRenderServices*		MaterialRenderServices;
};




