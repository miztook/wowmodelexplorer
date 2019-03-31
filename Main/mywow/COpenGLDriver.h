#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "IVideoDriver.h"

class COpenGLSceneStateServices;
class COpenGLShaderServices;
class COpenGLMaterialRenderServices;
class COpenGLExtension;
class COpenGLVertexDeclaration;

struct SGLProgram;
struct SWindowInfo;

class COpenGLDriver : public IVideoDriver
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLDriver);

public:
	COpenGLDriver();
	virtual ~COpenGLDriver();

public:
	bool initDriver(const SWindowInfo& wndInfo, uint32_t adapter, bool fullscreen, bool vsync, uint8_t antialias, bool multithread);

public:
	virtual E_DRIVER_TYPE getDriverType() const { return EDT_OPENGL; }
	virtual uint32_t getAdapterCount() const { return AdapterCount; }

	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color);

	virtual bool checkValid();
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

public:
	 COpenGLExtension*	getGLExtension() const { return GLExtension; }
	void deleteVao(CVertexBuffer* vbuffer);

private:
	struct SDeviceState				//除去material以外的state，material设置时已经做了cache
	{
		void reset();

		SColorf	clearColor;
		recti	viewport;
	};

private:
	dimension2du getWindowSize() const;
	void drawIndexedPrimitive( const SBufferParam& bufferParam, 
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount, 
		const SDrawParam& drawParam);

	int chooseMultiSamplePixelFormat(int pixelformat, uint8_t& antialias, PIXELFORMATDESCRIPTOR* ppfd);

	bool reset();
	void createVertexDecl();
	void releaseVertexDecl();
	void setVertexDeclarationAndBuffers(E_VERTEX_TYPE type, const SGLProgram* program, CVertexBuffer* vbuffer0, uint32_t offset0, CVertexBuffer* vbuffer1, uint32_t offset1, CIndexBuffer* ibuffer);

	COpenGLVertexDeclaration* getVertexDeclaration(E_VERTEX_TYPE type) const { return VertexDeclarations[type]; }
	void setRenderState3DMode(E_VERTEX_TYPE vType);
	void setRenderState2DMode(E_VERTEX_TYPE vType, const S2DBlendParam& blendParam );

	void logCaps();

public:
	//device settings
	window_type		HWnd;
	dc_type					Hdc;
	glcontext_type		Hgrc;

	ECOLOR_FORMAT ColorFormat;
	ECOLOR_FORMAT DepthFormat;
	
	GLint		DefaultFrameBuffer;
	//rendertarget
	IRenderTarget*		CurrentRenderTarget;

	CAdapterInfo	AdapterInfo;

private:
	E_RENDER_MODE		CurrentRenderMode;	

	// device state cache
	SDeviceState	CurrentDeviceState;
	IMaterialRenderer*		MaterialRenderer;
	COpenGLSceneStateServices*	OpenGLSceneStateServices;
	COpenGLShaderServices*		OpenGLShaderServices;
	COpenGLMaterialRenderServices*		OpenGLMaterialRenderServices;

	COpenGLExtension*		GLExtension;
	COpenGLVertexDeclaration*		VertexDeclarations[EVT_COUNT];

	uint32_t		AdapterCount;
	uint32_t		PrimitivesDrawn;
	uint32_t		DrawCall;

	char		DebugMsg[512];

	bool		ResetRenderStates;

	friend class COpenGLHardwareBufferServices;
	friend class COpenGLMaterialRenderServices;
};

#endif