#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "IVideoDriver.h"

#include <egl/egl.h>

class CGLES2Platform;
class CGLES2SceneStateServices;
class CGLES2ShaderServices;
class CGLES2MaterialRenderServices;
class CGLES2Extension;
class CGLES2VertexDeclaration;

struct SGLProgram; 
struct SWindowInfo;

class CGLES2Driver : public IVideoDriver
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2Driver);

public:
	CGLES2Driver();
	virtual ~CGLES2Driver();

public:
	bool initDriver(const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread);

public:
	virtual E_DRIVER_TYPE getDriverType() const { return EDT_GLES2; }
	virtual u32 getAdapterCount() const { return AdapterCount; }

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color);

	virtual bool checkValid();
	virtual bool setRenderTarget( IRenderTarget* texture );

	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat );
	virtual void setViewPort( recti area );
	virtual void setDisplayMode(const dimension2du& size);
	virtual bool setDriverSetting(const SDriverSetting& setting);

	virtual void drawDebugInfo(const c8* strMsg);

	//helper functions
	virtual void helper_render(CMeshRenderer* meshRenderer, SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_render(CTerrainRenderer* terrainRenderer, SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_render(CTransluscentRenderer* transluscentRenderer, SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_render(CWmoRenderer* wmoRenderer, SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_render(CAlphaTestMeshRenderer* meshRenderer, SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_render(CAlphaTestWmoRenderer* wmoRenderer, SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_renderAllBatches(CParticleRenderer* particleRenderer, SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_renderAllBatches(CRibbonRenderer* ribbonRenderer, SRenderUnit*& currentUnit,  ICamera* cam);
	virtual void helper_renderAllBatches(CMeshDecalRenderer* meshDecalRenderer, SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_renderAllBatches(CTransluscentDecalRenderer* transDecalRenderer, SRenderUnit*& currentUnit, ICamera* cam);
	virtual void helper_renderAllBatches(CAlphaTestDecalRenderer* meshDecalRenderer, SRenderUnit*& currentUnit, ICamera* cam);

public:
	bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

	void setTexture(u32 stage, ITexture* texture);
	ITexture* getTexture(u32 index) const;
	void registerLostReset( ILostResetCallback* callback );
	void removeLostReset( ILostResetCallback* callback );

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
		ITexture* textures[],
		u32 numTextures);

	void setTransform_Material_Textures(const matrix4& matWorld,
		const matrix4& matView, 
		const matrix4& matProjection,
		const SMaterial& material,
		ITexture* textures[],
		u32 numTextures);

public:
	 CGLES2Extension* getGLExtension() const { return GLExtension; }
	void deleteVao(IVertexBuffer* vbuffer);

private:
	struct SDeviceState				//除去material以外的state，material设置时已经做了cache
	{
		void reset();

		SColorf		clearColor;
		recti	viewport;
	};

private:
	void drawIndexedPrimitive( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		u32 primCount, 
		const SDrawParam& drawParam);
	bool reset();
	void createVertexDecl();
	void releaseVertexDecl();
	void setVertexDeclarationAndBuffers(E_VERTEX_TYPE type, const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1, IIndexBuffer* ibuffer);

	CGLES2VertexDeclaration* getVertexDeclaration(E_VERTEX_TYPE type) const { return VertexDeclarations[type]; }
	void setRenderState3DMode(E_VERTEX_TYPE vType);
	void setRenderState2DMode(E_VERTEX_TYPE vType, const S2DBlendParam& blendParam );

	void logCaps();

public:
	//device settings
	CGLES2Platform*		GLPlatform;

	ECOLOR_FORMAT ColorFormat;

	GLint		DefaultFrameBuffer;
	//rendertarget
	IRenderTarget*		CurrentRenderTarget;

	CAdapterInfo	AdapterInfo;

private:
	E_RENDER_MODE		CurrentRenderMode;

	// device state cache
	SDeviceState	CurrentDeviceState;
	IMaterialRenderer*		MaterialRenderer;
	CGLES2SceneStateServices*		GLES2SceneStateServices;
	CGLES2ShaderServices*	GLES2ShaderServices;
	CGLES2MaterialRenderServices*		GLES2MaterialRenderServices;

	CGLES2Extension*		GLExtension;
	CGLES2VertexDeclaration*		VertexDeclarations[EVT_COUNT];

	u32		AdapterCount;
	u32		PrimitivesDrawn;
	u32		DrawCall;

	typedef std::list< ILostResetCallback*, qzone_allocator<ILostResetCallback*> > T_LostResetList;
	T_LostResetList	LostResetList;

	c8		DebugMsg[512];

	bool		ResetRenderStates;
	u8	Padding[3];

	friend class CGLES2HardwareBufferServices;
	friend class CGLES2MaterialRenderServices;
};

#endif