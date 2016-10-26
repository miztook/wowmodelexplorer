#include "stdafx.h"
#include "CGLES2Driver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Platform.h"
#include "CGLES2Helper.h"
#include "CGLES2Extension.h"
#include "CGLES2Shader.h"
#include "CGLES2ShaderServices.h"
#include "CGLES2MaterialRenderServices.h"
#include "CGLES2SceneStateServices.h"
#include "CGLES2VertexDeclaration.h"
#include "CGLES2RenderTarget.h"

#define  DEVICE_SET_CLEARCOLOR(prop, v)	if (CurrentDeviceState.prop != (v))		\
		{			\
		glClearColor(v.r, v.g, v.b, v.a); 	\
		ASSERT_GLES2_SUCCESS();			\
		CurrentDeviceState.prop = (v);	}

#define DEVICE_SET_VIEWPORT(prop, v) if (CurrentDeviceState.prop != (v))		\
		{	\
		glViewport(v.UpperLeftCorner.X, v.UpperLeftCorner.Y, v.getWidth(), v.getHeight()); 	\
		ASSERT_GLES2_SUCCESS();			\
		CurrentDeviceState.prop = (v);	}

CGLES2Driver::CGLES2Driver()
{	
	GLPlatform = new CGLES2Platform;
	
	ColorFormat = ECF_A8R8G8B8;

	PrimitivesDrawn = 0;
	DrawCall = 0;

	CurrentRenderMode = ERM_NONE;
	ResetRenderStates = true;

	DefaultFrameBuffer = 0;
	CurrentRenderTarget = NULL;

	GLExtension = new CGLES2Extension;

	MaterialRenderer = NULL;
	ShaderServices = NULL;
	MaterialRenderServices = NULL;
	SceneStateServices = NULL;
	GLES2ShaderServices = NULL;
	GLES2MaterialRenderServices = NULL;
	GLES2SceneStateServices = NULL;

	AdapterCount = 0;

	//2D
	InitMaterial2D.MaterialType = EMT_2D;
	InitMaterial2D.Cull = ECM_BACK;
	InitMaterial2D.Lighting = false;
	InitMaterial2D.ZWriteEnable = false;
	InitMaterial2D.ZBuffer = ECFN_NEVER;
	InitMaterial2D.AntiAliasing = EAAM_OFF;
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		InitMaterial2D.TextureLayer[i].TextureWrapU=ETC_CLAMP;
		InitMaterial2D.TextureLayer[i].TextureWrapV=ETC_CLAMP;
		InitMaterial2D.TextureLayer[i].UseTextureMatrix = false;

		InitOverrideMaterial2D.TextureFilters[i] = ETF_NONE;
	}
	InitMaterial2DZTest = InitMaterial2D;
	InitMaterial2DZTest.ZBuffer = ECFN_LESSEQUAL;
	InitMaterial2DScissorTest = InitMaterial2D;
	InitMaterial2DScissorTest.ScissorEnable = true;
	InitMaterial2DScissorZTest = InitMaterial2D;
	InitMaterial2DScissorZTest.ScissorEnable = true;
	InitMaterial2DScissorZTest.ZBuffer = ECFN_LESSEQUAL;

	memset(VertexDeclarations, 0, sizeof(VertexDeclarations));
	memset(DebugMsg, 0, sizeof(DebugMsg));
}

CGLES2Driver::~CGLES2Driver()
{
	releaseVertexDecl();

	delete		SceneStateServices;
	delete		MaterialRenderServices;
	delete		ShaderServices;
	
	delete		GLExtension;	
	delete		GLPlatform;
}

bool CGLES2Driver::initDriver( const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{	
	DriverSetting.vsync = vsync;
	DriverSetting.fullscreen = fullscreen;
	
	if (!GLPlatform->initDriver(wndInfo, adapter, fullscreen, vsync, antialias, multithread))
	{
		CSysUtility::messageBoxWarning("GLES2 initialize failed!");
		return false;
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: GLES2");

	dimension2du windowSize = GLPlatform->getInitWindowSize();

	//device info
	AdapterCount = 1;
	AdapterInfo.index = 0;

	const c8* str = reinterpret_cast<const c8*>(glGetString(GL_RENDERER));
	Q_strcpy(AdapterInfo.description, 512, str);
	const c8* str1 = reinterpret_cast<const c8*>(glGetString(GL_VERSION));
	Q_sprintf(AdapterInfo.name, DEFAULT_SIZE, "GLES2 %s", str1);
	const c8* str2 = reinterpret_cast<const c8*>(glGetString(GL_VENDOR));
	Q_strcpy(AdapterInfo.vendorName, DEFAULT_SIZE, str2);

	//log
	g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Adapter Name: %s", AdapterInfo.name);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Adpater Description: %s", AdapterInfo.description);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Adapter Vendor: %s", AdapterInfo.vendorName);

	if (!GLExtension->initExtensions())
	{
		CSysUtility::messageBoxWarning("Video Card Extension does not support some features. Application will now exit!");
		return false;
	}
	g_Engine->getFileSystem()->writeLog(ELOG_GX,  "GLES2 driver Version is %0.2f", GLExtension->Version * 0.01f);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "GLES2 shader language Version is %d", GLExtension->ShaderLanguageVersion);
	
	//antialias
	if (antialias > 0)
	{
		if (antialias * 2 > GLExtension->MaxMultiSample)
			antialias = GLExtension->MaxMultiSample / 2;
	}

	DriverSetting.antialias = antialias;
	DriverSetting.quality = 0;

	logCaps();

	if (!queryFeature(EVDF_PIXEL_SHADER_2_0) ||		//ps_1_1 not allowed
		!queryFeature(EVDF_RENDER_TO_TARGET) ||
		!queryFeature(EVDF_MIP_MAP) ||
		!queryFeature(EVDF_TEXTURE_ADDRESS) ||
		!queryFeature(EVDF_SEPARATE_UVWRAP) ||
		!GLExtension->TextureCompression)
	{
		CSysUtility::messageBoxWarning("Video Card does not support some features. Application will now exit!");
		return false;
	}

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &DefaultFrameBuffer);

#ifdef MW_PLATFORM_IOS
    if(DefaultFrameBuffer == 0)
    {
        //did you forget [view bindDrawable]? mobile devices don't have default framebuffer as 0
        
        g_Engine->getFileSystem()->writeLog(ELOG_GX, "Invalid default fbo value 0");
        ASSERT(false);
        return false;
    }
#endif
    
	ColorFormat = ECF_A8R8G8B8;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	
	ASSERT_GLES2_SUCCESS();

	//shader, material
	ShaderServices = 
		GLES2ShaderServices = new CGLES2ShaderServices;
	MaterialRenderServices = 
		GLES2MaterialRenderServices = new CGLES2MaterialRenderServices;
	SceneStateServices = 
		GLES2SceneStateServices = new CGLES2SceneStateServices;

	createVertexDecl();	

	setDisplayMode(windowSize);

	return true;
}

void CGLES2Driver::logCaps()
{
	IFileSystem* fs = g_Engine->getFileSystem();

	fs->writeLog(ELOG_GX, "Device Caps:");
	if (queryFeature(EVDF_PIXEL_SHADER_3_0))
		fs->writeLog(ELOG_GX, "\tPixel Shader: 3.0");
	else if (queryFeature(EVDF_PIXEL_SHADER_2_0))
		fs->writeLog(ELOG_GX, "\tPixel Shader: 2.0");
	else
		fs->writeLog(ELOG_GX, "\tPixel Shader 2.0 minimum not supported!");

	if (queryFeature(EVDF_VERTEX_SHADER_3_0))
		fs->writeLog(ELOG_GX, "\tVertex Shader: 3.0");
	else if (queryFeature(EVDF_VERTEX_SHADER_2_0))
		fs->writeLog(ELOG_GX, "\tVertex Shader: 2.0");
	else
		fs->writeLog(ELOG_GX, "\tVertex Shader 2.0 minimum not supported!");

	if (GLExtension->TextureCompressionS3)
		fs->writeLog(ELOG_GX, "\tTexture Compression s3tc supported.");

	if (GLExtension->TextureCompressionPVR)
		fs->writeLog(ELOG_GX, "\tTexture Compression pvrtc supported.");

	if (GLExtension->TextureCompressionPVR2)
		fs->writeLog(ELOG_GX, "\tTexture Compression pvr2tc supported.");

	if (GLExtension->TextureCompressionETC1)
		fs->writeLog(ELOG_GX, "\tTexture Compression etc1 supported.");

	if (GLExtension->TextureCompressionATC)
		fs->writeLog(ELOG_GX, "\tTexture Compression atc supported.");

	if (GLExtension->TextureCompression3DC)
		fs->writeLog(ELOG_GX, "\tTexture Compression 3dc supported.");

	if (!GLExtension->TextureCompression)
		fs->writeLog(ELOG_GX, "\tTexture Compression not supported.");
}

bool CGLES2Driver::checkValid()
{
	return true;
}

bool CGLES2Driver::beginScene()
{
	PrimitivesDrawn = 0;
	DrawCall = 0;

	return true;
}

bool CGLES2Driver::endScene()
{
	if (GLExtension->DiscardFrameSupported)		//discard frame
	{
		const GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
		GLExtension->extGlDiscardFramebufferEXT(GL_FRAMEBUFFER, 3, attachments);
	}

	return GLPlatform->swapBuffers();
}

bool CGLES2Driver::clear( bool renderTarget, bool zBuffer, bool stencil, SColor color )
{
	GLbitfield mask = 0;

	if (renderTarget)
		mask |= GL_COLOR_BUFFER_BIT;

	if (zBuffer)
	{
		GLES2MaterialRenderServices->setZWriteEnable(true);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (stencil)
		mask |= GL_STENCIL_BUFFER_BIT;

	if (mask)
	{
		DEVICE_SET_CLEARCOLOR(clearColor, SColorf(color));

		glClear(mask);
		ASSERT_GLES2_SUCCESS();
	}

	return true;
}

bool CGLES2Driver::queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
{
	return GLExtension->queryFeature(feature);
}

void CGLES2Driver::setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat )
{
	ASSERT( 0 <= state && state < ETS_COUNT );
	switch( state )
	{
	case ETS_VIEW:
		{
			Matrices[ETS_VIEW] = mat;

			WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}
		break;
	case ETS_WORLD:
		{
			Matrices[ETS_WORLD] = mat;

			WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}
		break;

	case ETS_PROJECTION:
		{
			Matrices[ETS_PROJECTION] = mat;

			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}	
		break;
	default:		//texture
		{
			s32 tex = state - ETS_TEXTURE_0;
			if (  tex >= 0  && tex < MATERIAL_MAX_TEXTURES )
			{
				Matrices[state] = mat;
			}
		} 
		break;
	}
}

void CGLES2Driver::setTexture( u32 stage, ITexture* texture )
{
	GLES2MaterialRenderServices->setSampler_Texture(stage, texture);
}

void CGLES2Driver::setTransform(const matrix4& matView, const matrix4& matProjection)
{
	Matrices[ETS_VIEW] = matView;
	Matrices[ETS_PROJECTION] = matProjection;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;
}

void CGLES2Driver::setTransform_Material_Textures( const matrix4& matWorld, const SMaterial& material, ITexture* textures[], u32 numTextures )
{
	Matrices[ETS_WORLD] = matWorld;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;

	Material = material;

	u32 n = min_(numTextures, (u32)MATERIAL_MAX_TEXTURES);
	for (u32 i=0; i<n; ++i)
	{
		GLES2MaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

void CGLES2Driver::setTransform_Material_Textures( const matrix4& matWorld, const matrix4& matView, const matrix4& matProjection, const SMaterial& material, ITexture* textures[], u32 numTextures )
{
	Matrices[ETS_WORLD] = matWorld;
	Matrices[ETS_VIEW] = matView;
	Matrices[ETS_PROJECTION] = matProjection;

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;

	Material = material;

	u32 n = min_(numTextures, (u32)MATERIAL_MAX_TEXTURES);
	for (u32 i=0; i<n; ++i)
	{
		GLES2MaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

ITexture* CGLES2Driver::getTexture( u32 index ) const
{
	return GLES2MaterialRenderServices->getSampler_Texture(index);
}

bool CGLES2Driver::setRenderTarget( IRenderTarget* texture )
{
	CGLES2RenderTarget* tex = static_cast<CGLES2RenderTarget*>(texture);

	if ( tex == NULL )
	{
		if (GLExtension->DiscardFrameSupported)		//discard frame
        {
			const GLenum attachments[] = { GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
			GLExtension->extGlDiscardFramebufferEXT(GL_FRAMEBUFFER, 2, attachments);
		}

		GLExtension->extGlBindFramebuffer(GL_FRAMEBUFFER, DefaultFrameBuffer);		//restore
	}
	else
	{
		GLExtension->extGlBindFramebuffer(GL_FRAMEBUFFER, tex->getFrameBuffer());
	}

	bool success = GLExtension->checkFBOStatus();
	ASSERT(success);

	CurrentRenderTarget = texture;

	//ResetRenderStates = true;

	return true;
}

void CGLES2Driver::setViewPort( recti area )
{
	recti vp = area;
	vp.clipAgainst(recti(0,0,ScreenSize.Width,ScreenSize.Height));
	if ( vp.getWidth() <= 0 || vp.getHeight() <= 0 )
		return;

	DEVICE_SET_VIEWPORT(viewport, vp);

	Viewport = vp;
}

void CGLES2Driver::registerLostReset( ILostResetCallback* callback )
{
	if (std::find(LostResetList.begin(), LostResetList.end(), callback) == LostResetList.end())
	{
		LostResetList.push_back(callback);
	}
}

void CGLES2Driver::removeLostReset( ILostResetCallback* callback )
{
	LostResetList.remove(callback);
}

void CGLES2Driver::SDeviceState::reset()
{
	glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clearColor);

	GLint v[4];
	glGetIntegerv(GL_VIEWPORT, v);
	viewport = recti(v[0], v[1], v[0]+v[2], v[1]+v[3]);
}

bool CGLES2Driver::reset()
{
	for( T_LostResetList::const_iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onLost();

	for( T_LostResetList::const_iterator itr=LostResetList.begin(); itr != LostResetList.end(); ++itr )
		(*itr)->onReset();

	//reset
	LastMaterial.MaterialType = (E_MATERIAL_TYPE)-1;
	ResetRenderStates = true;
	CurrentRenderMode = ERM_NONE;
	CurrentDeviceState.reset();

	GLES2SceneStateServices->reset();

	dimension2du windowSize = ScreenSize;
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "reset... Device Format: %dX%d, %s", windowSize.Width, windowSize.Height, getColorFormatString(ColorFormat));

	return true;
}

void CGLES2Driver::setDisplayMode( const dimension2du& size )
{
	ScreenSize = size;

	reset();

	setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));
}

bool CGLES2Driver::setDriverSetting( const SDriverSetting& setting )
{
	bool vsync = setting.vsync;
	//u8 antialias = setting.antialias;
	//bool fullscreen = setting.fullscreen;

	if (vsync != DriverSetting.vsync)
	{
		GLPlatform->applyVsync(vsync);

		DriverSetting.vsync = vsync;
	}

	return true;
}

void CGLES2Driver::createVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		VertexDeclarations[i] = new CGLES2VertexDeclaration((E_VERTEX_TYPE)i);
	}
}

void CGLES2Driver::releaseVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		delete VertexDeclarations[i];
	}
}

void CGLES2Driver::setVertexDeclarationAndBuffers( E_VERTEX_TYPE type, const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1, IIndexBuffer* ibuffer )
{
	CGLES2VertexDeclaration* decl = getVertexDeclaration(type);
	ASSERT(decl);
	decl->apply(program, vbuffer0, offset0, vbuffer1, offset1);

	if (ibuffer)
	{
		ASSERT(ibuffer->HWLink);
		GLExtension->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(ibuffer->HWLink));
	}
}

void CGLES2Driver::deleteVao(IVertexBuffer* vbuffer)
{
	E_VERTEX_TYPE vType = getVertexType(vbuffer->Type);
	if (vType != EVT_INVALID)
	{
		VertexDeclarations[vType]->deleteVao(vbuffer);
	}
}

#endif