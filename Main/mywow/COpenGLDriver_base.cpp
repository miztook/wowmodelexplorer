#include "stdafx.h"
#include "COpenGLDriver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "GL\wglext.h"
#include "COpenGLHelper.h"
#include "COpenGLExtension.h"
#include "COpenGLShader.h"
#include "COpenGLShaderServices.h"
#include "COpenGLMaterialRenderServices.h"
#include "COpenGLSceneStateServices.h"
#include "COpenGLVertexDeclaration.h"
#include "COpenGLRenderTarget.h"

#define  DEVICE_SET_CLEARCOLOR(prop, v)	if (CurrentDeviceState.prop != (v))		\
		{			\
			glClearColor(v.r, v.g, v.b, v.a); 	\
		ASSERT_OPENGL_SUCCESS();			\
		CurrentDeviceState.prop = (v);	}	

#define DEVICE_SET_VIEWPORT(prop, v) if (CurrentDeviceState.prop != (v))		\
		{	\
		glViewport(v.UpperLeftCorner.X, v.UpperLeftCorner.Y, v.getWidth(), v.getHeight()); 	\
		ASSERT_OPENGL_SUCCESS();			\
		CurrentDeviceState.prop = (v);	}	

COpenGLDriver::COpenGLDriver()
{
	HWnd = NULL_PTR;
	Hdc = NULL_PTR;
	Hgrc = NULL_PTR;

	ColorFormat = ECF_A8R8G8B8;
	DepthFormat = ECF_D24S8;

	PrimitivesDrawn = 0;
	DrawCall = 0;

	CurrentRenderMode = ERM_NONE;
	ResetRenderStates = true;
	DefaultFrameBuffer = 0;
	CurrentRenderTarget = NULL_PTR;

	GLExtension = new COpenGLExtension;

	MaterialRenderer = NULL_PTR;
	ShaderServices = NULL_PTR;
	MaterialRenderServices = NULL_PTR;
	SceneStateServices = NULL_PTR;
	OpenGLShaderServices = NULL_PTR;
	OpenGLMaterialRenderServices = NULL_PTR;
	OpenGLSceneStateServices = NULL_PTR;
	
	AdapterCount = 0;

	//2D
	InitMaterial2D.MaterialType = EMT_2D;
	InitMaterial2D.Cull = ECM_BACK;
	InitMaterial2D.Lighting = false;
	InitMaterial2D.ZWriteEnable = false;
	InitMaterial2D.ZBuffer = ECFN_NEVER;
	InitMaterial2D.AntiAliasing = EAAM_LINE_SMOOTH;
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

COpenGLDriver::~COpenGLDriver()
{
	releaseVertexDecl();

	delete		SceneStateServices;
	delete		MaterialRenderServices;
	delete		ShaderServices;

	delete		GLExtension;

	if (!wglMakeCurrent(NULL_PTR, NULL_PTR))
	{
		ASSERT(false);
	}

	if (Hgrc && !wglDeleteContext(Hgrc))
	{
		ASSERT(false);
	}

	if (Hdc && HWnd)
		::ReleaseDC(HWnd, Hdc);
}

bool COpenGLDriver::initDriver( const SWindowInfo& wndInfo, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{
	ASSERT(::IsWindow(wndInfo.hwnd));

	HWnd = wndInfo.hwnd;

	DriverSetting.vsync = vsync;
	DriverSetting.fullscreen = fullscreen;

	dimension2du windowSize;

	//changeWindowSize
	if (DriverSetting.fullscreen)
	{
		windowSize = dimension2du(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
		SetWindowLongPtrA(HWnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		::MoveWindow(HWnd, 0, 0, windowSize.Width, windowSize.Height, TRUE);
	}
	else
	{
		windowSize = getWindowSize();
	}
	
	Hdc = ::GetDC(HWnd);

	const u8 depthBits = 24;
	const u8 stencilBits = 8;

	// Set up pixel format descriptor with desired parameters
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),             // Size Of This Pixel Format Descriptor
		1,                                         // Version Number
		PFD_DRAW_TO_WINDOW |                       // Format Must Support Window
		PFD_SUPPORT_OPENGL |                       // Format Must Support OpenGL
		PFD_DOUBLEBUFFER | // Must Support Double Buffering
	//	PFD_STEREO,        // Must Support Stereo Buffer
		PFD_TYPE_RGBA,                             // Request An RGBA Format
		32,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                          // Color Bits Ignored
		0,                                         // No Alpha Buffer
		0,                                         // Shift Bit Ignored
		0,                                         // No Accumulation Buffer
		0, 0, 0, 0,	                               // Accumulation Bits Ignored
		depthBits,                        // Z-Buffer (Depth Buffer)
		stencilBits,              // Stencil Buffer Depth
		0,                                         // No Auxiliary Buffer
		PFD_MAIN_PLANE,                            // Main Drawing Layer
		0,                                         // Reserved
		0, 0, 0                                    // Layer Masks Ignored
	};

	//choose pixel format
	int pixelFormat;

	//depth stencil
	pixelFormat = ChoosePixelFormat(Hdc, &pfd);
	DepthFormat = ECF_D24S8;
	if (!pixelFormat)
	{
		pfd.cDepthBits = 15;
		pfd.cStencilBits = 1;

		pixelFormat = ChoosePixelFormat(Hdc, &pfd);
		DepthFormat = ECF_D16;
	}
	
	if (!pixelFormat)
	{
		ASSERT(false);
		return false;
	}

	if (!SetPixelFormat(Hdc, pixelFormat, &pfd))
	{
		ASSERT(false);
		return false;
	}

	Hgrc = wglCreateContext(Hdc);
	if (!Hgrc)
	{
		::ReleaseDC(HWnd, Hdc);

		ASSERT(false);
		return false;
	}

	if (!wglMakeCurrent(Hdc, Hgrc))
	{
		wglDeleteContext(Hgrc);
		::ReleaseDC(HWnd, Hdc);
		
		return false;
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX,  "Render System: OpenGL");

	//device info
	AdapterCount = 1;
	AdapterInfo.index = 0;

	const c8* str = reinterpret_cast<const c8*>(glGetString(GL_RENDERER));
	Q_strcpy(AdapterInfo.description, 512, str);
	const c8* str1 = reinterpret_cast<const c8*>(glGetString(GL_VERSION));
	Q_sprintf(AdapterInfo.name, DEFAULT_SIZE, "OpenGL %s", str1);
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
	g_Engine->getFileSystem()->writeLog(ELOG_GX,  "OpenGL driver Version is %0.2f", GLExtension->Version * 0.01f);
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "OpenGL shader language Version is %d", GLExtension->ShaderLanguageVersion);

	//antialias
	if (antialias > 0)
	{
		if (antialias * 2 > GLExtension->MaxMultiSample)
			antialias = GLExtension->MaxMultiSample / 2;
	}

	DriverSetting.antialias = antialias;
	DriverSetting.quality = 0;

	//vsync
#ifdef WGL_EXT_swap_control
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	// vsync extension
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	// set vsync
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(vsync ? 1 : 0);
#endif	

	logCaps();

	if (!queryFeature(EVDF_PIXEL_SHADER_2_0) ||		//ps_1_1 not allowed
		!queryFeature(EVDF_STREAM_OFFSET) ||
		!queryFeature(EVDF_RENDER_TO_TARGET) ||
		!queryFeature(EVDF_MIP_MAP) ||
		!queryFeature(EVDF_TEXTURE_ADDRESS) ||
		!queryFeature(EVDF_SEPARATE_UVWRAP) ||
		!GLExtension->TextureCompressionS3 ||
		!GLExtension->queryOpenGLFeature(IRR_EXT_packed_depth_stencil))
	{
		CSysUtility::messageBoxWarning("Video Card does not support some features. Application will now exit!");
		return false;
	}

	//backbuffer format
	int pf = GetPixelFormat(Hdc);
	DescribePixelFormat(Hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (pfd.cAlphaBits != 0)
	{
		if (pfd.cRedBits == 8)
			ColorFormat = ECF_A8R8G8B8;
		else if(pfd.cGreenBits == 5)
			ColorFormat = ECF_A1R5G5B5;
	}
	else
	{
		if (pfd.cRedBits == 8)
			ColorFormat = ECF_A8R8G8B8;
		else if(pfd.cGreenBits == 6)
			ColorFormat = ECF_R5G6B5;
		else if(pfd.cGreenBits == 5)
			ColorFormat = ECF_A1R5G5B5;
	}

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &DefaultFrameBuffer);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	ASSERT_OPENGL_SUCCESS();

	//shader, material
 	ShaderServices = 
	OpenGLShaderServices = new COpenGLShaderServices;

 	MaterialRenderServices = 
	OpenGLMaterialRenderServices = new COpenGLMaterialRenderServices;

 	SceneStateServices = 
	OpenGLSceneStateServices = new COpenGLSceneStateServices;


	createVertexDecl();	

	setDisplayMode(windowSize);

	return true;
}

int COpenGLDriver::chooseMultiSamplePixelFormat( int pixelformat, u8& antialias, PIXELFORMATDESCRIPTOR* ppfd)
{
	glcontext_type rc, rc_before=wglGetCurrentContext();
	window_type hWnd;
	dc_type hDC, hDC_before=wglGetCurrentDC();
	WNDCLASS wndCls;
	HINSTANCE inst = GetModuleHandle(NULL_PTR);

	/* create a dummy window */
	ZeroMemory(&wndCls, sizeof(wndCls));
	wndCls.lpfnWndProc = DefWindowProc;
	wndCls.hInstance = inst;
	wndCls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wndCls.lpszClassName = "dummy";
	RegisterClass( &wndCls );

	hWnd=CreateWindow("dummy", "", WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW , 0, 0, 0, 0, 0, 0, inst, 0 );
	hDC=GetDC(hWnd);
	SetPixelFormat( hDC, pixelformat, ppfd );

	rc = wglCreateContext( hDC );
	wglMakeCurrent(hDC, rc);

#ifdef WGL_ARB_pixel_format
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormat_ARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if (wglChoosePixelFormat_ARB)
	{
		if(antialias > 16)
			antialias = 16;

		f32 fAttributes[] = {0.0, 0.0};
		s32 iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,	GL_TRUE,
			WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,	ppfd->cColorBits,
			WGL_ALPHA_BITS_ARB,	ppfd->cAlphaBits,
			WGL_DEPTH_BITS_ARB,	ppfd->cDepthBits, // 10,11
			WGL_STENCIL_BITS_ARB,	ppfd->cStencilBits,
			WGL_DOUBLE_BUFFER_ARB,	(ppfd->dwFlags & PFD_DOUBLEBUFFER) ? GL_TRUE : GL_FALSE,
			WGL_STEREO_ARB,		(ppfd->dwFlags & PFD_STEREO) ? GL_TRUE : GL_FALSE,
#ifdef WGL_ARB_multisample
			WGL_SAMPLE_BUFFERS_ARB, 1,
			WGL_SAMPLES_ARB,	antialias * 2, // 20,21
#elif defined(WGL_EXT_multisample)
			WGL_SAMPLE_BUFFERS_EXT, 1,
			WGL_SAMPLES_EXT,	antialias * 2, // 20,21
#elif defined(WGL_3DFX_multisample)
			WGL_SAMPLE_BUFFERS_3DFX, 1,
			WGL_SAMPLES_3DFX,		antialias * 2, // 20,21
#endif
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			0,0
		};

		s32 rv=0;
		// Try to get an acceptable pixel format
		while(rv==0 && iAttributes[21] > 1)
		{
			s32 outpf=0;
			u32 numFormats=0;
			const s32 valid = wglChoosePixelFormat_ARB(Hdc, iAttributes, fAttributes, 1, &outpf, &numFormats);

			if (valid && numFormats>0)
				rv = outpf;
			else
				iAttributes[21] -= 1;
		}
		if (rv)
		{
			pixelformat =rv;
			antialias = iAttributes[21] / 2;
		}
		else
		{
			antialias = 0;
		}
	}
	else
#endif
	{
		antialias = 0;
	}

	wglMakeCurrent( hDC_before, rc_before);
	wglDeleteContext(rc);
	ReleaseDC(hWnd, hDC);
	DestroyWindow(hWnd);
	UnregisterClass("dummy", inst);

	return pixelformat;
}

dimension2du COpenGLDriver::getWindowSize() const
{
	RECT rc;
	::GetClientRect(HWnd, &rc);
	return dimension2du((u32)rc.right-rc.left, (u32)rc.bottom-rc.top);
}

void COpenGLDriver::logCaps()
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

	if (GLExtension->TextureCompressionETC1)
		fs->writeLog(ELOG_GX, "\tTexture Compression etc1 supported.");

	if (GLExtension->TextureCompressionATC)
		fs->writeLog(ELOG_GX, "\tTexture Compression atc supported.");

	if (GLExtension->TextureCompression3DC)
		fs->writeLog(ELOG_GX, "\tTexture Compression 3dc supported.");

	if (!GLExtension->TextureCompression)
		fs->writeLog(ELOG_GX, "\tTexture Compression not supported.");
	
}

bool COpenGLDriver::checkValid()
{
	return true;
}

bool COpenGLDriver::beginScene()
{
	PrimitivesDrawn = 0;
	DrawCall = 0;

	return true;
}

bool COpenGLDriver::endScene()
{
//	glFlush();
//	ASSERT_OPENGL_SUCCESS();

	//discard frame buffer
	if (GLExtension->DiscardFrameSupported)
	{
		if(CurrentRenderTarget != NULL_PTR)
		{
			const GLenum attachments[] = { GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
			GLExtension->extGlInvalidateFramebuffer(GL_FRAMEBUFFER_EXT, 2, attachments);
		}
		else
		{
			const GLenum attachments[] = { GL_DEPTH, GL_STENCIL };
			GLExtension->extGlInvalidateFramebuffer(GL_FRAMEBUFFER_EXT, 2, attachments);
		}
	}

	return SwapBuffers(Hdc) == TRUE;
}

bool COpenGLDriver::clear( bool renderTarget, bool zBuffer, bool stencil, SColor color )
{
	GLbitfield mask = 0;
	
	if (renderTarget)
		mask |= GL_COLOR_BUFFER_BIT;

	if (zBuffer)
	{
		OpenGLMaterialRenderServices->setZWriteEnable(true);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (stencil)
		mask |= GL_STENCIL_BUFFER_BIT;

	if (mask)
	{
		DEVICE_SET_CLEARCOLOR(clearColor, SColorf(color));

		glClear(mask);
		ASSERT_OPENGL_SUCCESS();
	}

	return true;
}

bool COpenGLDriver::queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
{
	return GLExtension->queryFeature(feature);
}

void COpenGLDriver::setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat )
{
	ASSERT( 0 <= state && state < ETS_COUNT );
	switch( state )
	{
	case ETS_VIEW:
		{
			Matrices[ETS_VIEW] = mat;

#ifdef FIXPIPELINE
			glMatrixMode(GL_MODELVIEW);
			ASSERT_OPENGL_SUCCESS();

			glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
			ASSERT_OPENGL_SUCCESS();
#endif

			WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}
		break;
	case ETS_WORLD:
		{
			Matrices[ETS_WORLD] = mat;

#ifdef FIXPIPELINE
			glMatrixMode(GL_MODELVIEW);
			ASSERT_OPENGL_SUCCESS();

			glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
			ASSERT_OPENGL_SUCCESS();
#endif

			WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
			WVP = WV * Matrices[ETS_PROJECTION];

			CurrentRenderMode = ERM_3D;
		}
		break;

	case ETS_PROJECTION:
		{
#ifdef FIXPIPELINE
			GLfloat glmat[16];
			COpenGLHelper::getGLMatrixFromD3D(glmat, mat);

			glMatrixMode(GL_PROJECTION);
			ASSERT_OPENGL_SUCCESS();

			glLoadMatrixf(glmat);
			ASSERT_OPENGL_SUCCESS();
#endif
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
#ifdef FIXPIPELINE
				OpenGLMaterialRenderServices->setActiveTexture(tex);
				glMatrixMode(GL_TEXTURE);
				ASSERT_OPENGL_SUCCESS();
				if (mat.isIdentity())
				{
					glLoadIdentity();
					ASSERT_OPENGL_SUCCESS();
				}
				else
				{
					GLfloat glmat[16];
					COpenGLHelper::getGLTextureMatrix(glmat, mat);
					glLoadMatrixf(glmat);
					ASSERT_OPENGL_SUCCESS();
				}
#endif
				Matrices[state] = mat;
			}
		} 
		break;
	}
}

void COpenGLDriver::setTexture( u32 stage, ITexture* texture )
{
	OpenGLMaterialRenderServices->setSampler_Texture(stage, texture);
}

void COpenGLDriver::setTransform(const matrix4& matView, const matrix4& matProjection)
{
	Matrices[ETS_VIEW] = matView;
	Matrices[ETS_PROJECTION] = matProjection;

#ifdef FIXPIPELINE
	glMatrixMode(GL_MODELVIEW);
	ASSERT_OPENGL_SUCCESS();

	glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
	ASSERT_OPENGL_SUCCESS();

	GLfloat glmat[16];
	COpenGLHelper::getGLMatrixFromD3D(glmat, matProjection);

	glMatrixMode(GL_PROJECTION);
	ASSERT_OPENGL_SUCCESS();

	glLoadMatrixf(glmat);
	ASSERT_OPENGL_SUCCESS();
#endif

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;
}

void COpenGLDriver::setTransform_Material_Textures( const matrix4& matWorld, const SMaterial& material, ITexture* const textures[], u32 numTextures )
{
	Matrices[ETS_WORLD] = matWorld;

#ifdef FIXPIPELINE
	glMatrixMode(GL_MODELVIEW);
	ASSERT_OPENGL_SUCCESS();

	glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
	ASSERT_OPENGL_SUCCESS();
#endif

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;

	Material = material;

	u32 n = min_(numTextures, (u32)MATERIAL_MAX_TEXTURES);
	for (u32 i=0; i<n; ++i)
	{
		OpenGLMaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

void COpenGLDriver::setTransform_Material_Textures( const matrix4& matWorld, const matrix4& matView, const matrix4& matProjection, const SMaterial& material, ITexture* const textures[], u32 numTextures )
{
	Matrices[ETS_WORLD] = matWorld;
	Matrices[ETS_VIEW] = matView;
	Matrices[ETS_PROJECTION] = matProjection;

#ifdef FIXPIPELINE
	glMatrixMode(GL_MODELVIEW);
	ASSERT_OPENGL_SUCCESS();

	glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
	ASSERT_OPENGL_SUCCESS();

	GLfloat glmat[16];
	COpenGLHelper::getGLMatrixFromD3D(glmat, matProjection);

	glMatrixMode(GL_PROJECTION);
	ASSERT_OPENGL_SUCCESS();

	glLoadMatrixf(glmat);
	ASSERT_OPENGL_SUCCESS();
#endif

	WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
	WVP = WV * Matrices[ETS_PROJECTION];

	CurrentRenderMode = ERM_3D;

	Material = material;

	u32 n = min_(numTextures, (u32)MATERIAL_MAX_TEXTURES);
	for (u32 i=0; i<n; ++i)
	{
		OpenGLMaterialRenderServices->setSampler_Texture(i, textures[i]);
	}
}

ITexture* COpenGLDriver::getTexture( u32 index ) const
{
	return OpenGLMaterialRenderServices->getSampler_Texture(index);
}

bool COpenGLDriver::setRenderTarget( IRenderTarget* texture )
{
	COpenGLRenderTarget* tex = static_cast<COpenGLRenderTarget*>(texture);

	if ( tex == NULL_PTR )
	{
		//discard frame buffer
		const GLenum attachments[] = { GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
		GLExtension->extGlInvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);

		GLExtension->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, DefaultFrameBuffer);		//restore
	}
	else
	{
		GLExtension->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, tex->getFrameBuffer());
	}

	bool success = GLExtension->checkFBOStatus();
	ASSERT(success);

	CurrentRenderTarget = texture;

	//ResetRenderStates = true;

	return true;
}

void COpenGLDriver::setViewPort( recti area )
{
	recti vp = area;
	vp.clipAgainst(recti(0,0,ScreenSize.Width,ScreenSize.Height));
	if ( vp.getWidth() <= 0 || vp.getHeight() <= 0 )
		return;

	DEVICE_SET_VIEWPORT(viewport, vp);

	Viewport = vp;
}

void COpenGLDriver::registerLostReset( ILostResetCallback* callback )
{
	if (std::find(LostResetList.begin(), LostResetList.end(), callback) == LostResetList.end())
	{
		LostResetList.push_back(callback);
	}
}

void COpenGLDriver::removeLostReset( ILostResetCallback* callback )
{
	LostResetList.remove(callback);
}

void COpenGLDriver::SDeviceState::reset()
{
	glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&clearColor);

	GLint v[4];
	glGetIntegerv(GL_VIEWPORT, v);
	viewport = recti(v[0], v[1], v[0]+v[2], v[1]+v[3]);
}

bool COpenGLDriver::reset()
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

	OpenGLSceneStateServices->reset();

	dimension2du windowSize = ScreenSize;
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "reset... Device Format: %dX%d, %s", windowSize.Width, windowSize.Height, getColorFormatString(ColorFormat));

	return true;
}

void COpenGLDriver::setDisplayMode( const dimension2du& size )
{
	ScreenSize = size;

	reset();

	setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));
}

bool COpenGLDriver::setDriverSetting( const SDriverSetting& setting )
{
	bool vsyncChanged = false;
	bool antialiasChanged = false;
	bool fullscreenChanged = false;

	bool vsync = setting.vsync;
	u8 antialias = setting.antialias;
	bool fullscreen = setting.fullscreen;

	if (vsync != DriverSetting.vsync)
	{
#ifdef WGL_EXT_swap_control
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
		// vsync extension
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		// set vsync
		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(vsync ? 1 : 0);
#endif		
		
		vsyncChanged = true;

		DriverSetting.vsync = vsync;
	}

	if (antialias != DriverSetting.antialias)
	{
		antialiasChanged = true;

		DriverSetting.antialias = antialias;
		DriverSetting.quality = 0;
	}

	bool ret = true;
	if (vsyncChanged || fullscreenChanged || antialiasChanged)
	{
		// 		if (beginScene())
		// 		{
		// 			clear(true, false, false, SColor(0,0,0));
		// 			endScene();
		// 		}
		ret = reset();

		setViewPort(recti(0,0,ScreenSize.Width, ScreenSize.Height));

		if(ret)
		{ 	
			g_Engine->getFileSystem()->writeLog(ELOG_GX, "Driver Setting Changed. Vsync: %s, Antialias: %d, %s", 
				DriverSetting.vsync ? "On" : "Off",
				(s32)DriverSetting.antialias, 
				"Window");
		}
	}
	return ret;
}

void COpenGLDriver::createVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		VertexDeclarations[i] = new COpenGLVertexDeclaration((E_VERTEX_TYPE)i);
	}
}

void COpenGLDriver::releaseVertexDecl()
{
	for (u32 i=0; i<EVT_COUNT; ++i)
	{
		delete VertexDeclarations[i];
	}
}

void COpenGLDriver::setVertexDeclarationAndBuffers( E_VERTEX_TYPE type, const SGLProgram* program, IVertexBuffer* vbuffer0, u32 offset0, IVertexBuffer* vbuffer1, u32 offset1, IIndexBuffer* ibuffer )
{
	COpenGLVertexDeclaration* decl = getVertexDeclaration(type);
	ASSERT(decl);
	decl->apply(program, vbuffer0, offset0, vbuffer1, offset1);

	if (ibuffer)
	{
		ASSERT(ibuffer->HWLink);
		GLExtension->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)PTR_TO_UINT32(ibuffer->HWLink));
	}
}

void COpenGLDriver::deleteVao( IVertexBuffer* vbuffer )
{
	E_VERTEX_TYPE vType = getVertexType(vbuffer->Type);
	if (vType != EVT_INVALID)
	{
		VertexDeclarations[vType]->deleteVao(vbuffer);
	}
}

#endif