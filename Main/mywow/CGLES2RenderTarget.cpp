#include "stdafx.h"
#include "CGLES2RenderTarget.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"
#include "CGLES2Texture.h"
#include "CGLES2Helper.h"
#include "CGLES2MaterialRenderServices.h"

#ifdef MW_PLATFORM_IOS

CGLES2RenderTarget::CGLES2RenderTarget( dimension2du size, ECOLOR_FORMAT format )
	: FrameBuffer(0), CopyFrameBuffer(0), ColorSurface(0), DepthSurface(0), MultiSample(false)
{
	RTCopyTexture = new CGLES2Texture(false);

	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(size, format, setting.antialias, setting.quality);
	ASSERT(success);

	success = RTCopyTexture->createRTTexture(TextureSize, ColorFormat);
	ASSERT(success);

	bindTexture();
}

CGLES2RenderTarget::~CGLES2RenderTarget()
{
	RTCopyTexture->drop();

	releaseVideoTexture();
}

ITexture* CGLES2RenderTarget::getRTTexture() const
{
	return RTCopyTexture;
}

bool CGLES2RenderTarget::writeToRTTexture()
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2Extension* ext = driver->getGLExtension();
	if (MultiSample)
	{
		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

		ASSERT(CopyFrameBuffer);
		ext->extGlBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, FrameBuffer);
		ext->extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, CopyFrameBuffer);
		ext->extGlResolveMultisampleFramebuffer();

		driver->clear(false, true, false, SColor::Black());
		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

		GLenum pixelFormat;
		GLenum pixelType;

		GLint internalFormat = CGLES2Helper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

		//begin bind
		services->applySamplerTexture(0, RTCopyTexture);
		services->applyTextureFilter(0, ETF_NONE);
        services->applyTextureWrap(0, ETA_U, ETC_CLAMP);
        services->applyTextureWrap(0, ETA_V, ETC_CLAMP);
		services->applyTextureMipMap(0, false);

		glCopyTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 0, 0, TextureSize.Width, TextureSize.Height, 0);
		ASSERT_GLES2_SUCCESS();

		//end bind
		services->applySamplerTexture(0, NULL);

		driver->clear(false, true, false, SColor::Black());
		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	return true;
}

void CGLES2RenderTarget::onLost()
{
	RTCopyTexture->releaseVideoTexture();

	releaseVideoTexture();
}

void CGLES2RenderTarget::onReset()
{
	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(TextureSize, ColorFormat, setting.antialias, setting.quality);
	ASSERT(success);

	success = RTCopyTexture->createRTTexture(TextureSize, ColorFormat);
	ASSERT(success);

	bindTexture();
}

bool CGLES2RenderTarget::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format, u32 antialias, u32 quality )
{
	if (VideoBuilt)
		return false;

	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2Extension* ext = driver->getGLExtension();

	MultiSample = ext->MultisampleSupported && antialias > 0;

	TextureSize = size.getOptimalSize(!driver->queryFeature(EVDF_TEXTURE_NPOT), 
		!driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
		true, 
		driver->getGLExtension()->MaxTextureSize);

	ColorFormat = format;

	if(ColorFormat == ECF_UNKNOWN)
	{
		ColorFormat = driver->ColorFormat;
	}

	//
	ASSERT( !FrameBuffer && !ColorSurface && !DepthSurface && !CopyFrameBuffer);

	ext->extGlGenFramebuffers(1, &FrameBuffer);
	ext->extGlGenRenderbuffers(1, &ColorSurface);
	ext->extGlGenRenderbuffers(1, &DepthSurface);

	if (MultiSample)
		ext->extGlGenFramebuffers(1, &CopyFrameBuffer);

	ASSERT(ext->MaxMultiSample >= antialias * 2);

	ext->extGlBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	//color
	ext->extGlBindRenderbuffer(GL_RENDERBUFFER, ColorSurface);
	if (MultiSample)
		ext->extGlRenderbufferStorageMultisample(GL_RENDERBUFFER, antialias * 2, GL_RGBA8_OES, size.Width, size.Height);
	else
		ext->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, size.Width, size.Height);

	//depth
	ext->extGlBindRenderbuffer(GL_RENDERBUFFER, DepthSurface);
	if (MultiSample)
		ext->extGlRenderbufferStorageMultisample(GL_RENDERBUFFER, antialias * 2, GL_DEPTH_COMPONENT24_OES, size.Width, size.Height);
	else
		ext->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, size.Width, size.Height);

	ext->extGlBindRenderbuffer(GL_RENDERBUFFER, 0);

	ext->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorSurface);
	ext->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthSurface);

	bool success = ext->checkFBOStatus();
	if (!success)
	{
		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
		ASSERT(false);
		return false;
	}

	ext->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);

	VideoBuilt = true;
	return true;
}

void CGLES2RenderTarget::releaseVideoTexture()
{
	CGLES2Extension* ext = static_cast<CGLES2Driver*>(g_Engine->getDriver())->getGLExtension();
	if (DepthSurface)
	{
		ext->extGlDeleteRenderbuffers(1, &DepthSurface);
		DepthSurface = 0;
	}

	if (ColorSurface)
	{
		ext->extGlDeleteRenderbuffers(1, &ColorSurface);
		ColorSurface = 0;
	}

	if (CopyFrameBuffer)
	{
		ext->extGlDeleteFramebuffers(1, &CopyFrameBuffer);
		CopyFrameBuffer = 0;
	}

	if (FrameBuffer)
	{
		ext->extGlDeleteFramebuffers(1, &FrameBuffer);
		FrameBuffer = 0;
	}

	VideoBuilt = false;
}

void CGLES2RenderTarget::bindTexture()
{
	if (MultiSample)
	{
		CGLES2Extension* ext = static_cast<CGLES2Driver*>(g_Engine->getDriver())->getGLExtension();

		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, CopyFrameBuffer);
		ext->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RTCopyTexture->getGLTexture(), 0);

		bool success = ext->checkFBOStatus();
		ASSERT(success);

		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

#else

CGLES2RenderTarget::CGLES2RenderTarget( dimension2du size, ECOLOR_FORMAT format )
	: FrameBuffer(0), DepthSurface(0), MultiSample(false)
{
	RTCopyTexture = new CGLES2Texture(false);

	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(size, format, setting.antialias, setting.quality);
	ASSERT(success);
}

CGLES2RenderTarget::~CGLES2RenderTarget()
{
	releaseVideoTexture();
	RTCopyTexture->drop();
}

ITexture* CGLES2RenderTarget::getRTTexture() const
{
	return RTCopyTexture;
}

bool CGLES2RenderTarget::writeToRTTexture()
{
	return true;
}

void CGLES2RenderTarget::onLost()
{
 	releaseVideoTexture();
 	RTCopyTexture->releaseVideoTexture();
}

void CGLES2RenderTarget::onReset()
{
 	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
 	bool success = createAsRenderTarget(TextureSize, ColorFormat, setting.antialias, setting.quality);
 	ASSERT(success);
}

bool CGLES2RenderTarget::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format, u32 antialias, u32 quality )
{
	if (VideoBuilt)
		return false;

	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2Extension* ext = driver->getGLExtension();

	MultiSample = ext->MultisampleSupported && antialias > 0;

	TextureSize = size.getOptimalSize(!driver->queryFeature(EVDF_TEXTURE_NPOT), 
		!driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
		true, 
		driver->getGLExtension()->MaxTextureSize);

	ColorFormat = format;

	if(ColorFormat == ECF_UNKNOWN)
	{
		ColorFormat = driver->ColorFormat;
	}

	bool success = RTCopyTexture->createRTTexture(TextureSize, ColorFormat);
	ASSERT(success);

	//
	ASSERT( !FrameBuffer && !DepthSurface );

	ext->extGlGenFramebuffers(1, &FrameBuffer);
	ext->extGlGenRenderbuffers(1, &DepthSurface);

	ASSERT(ext->MaxMultiSample >= antialias * 2);

	ext->extGlBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	//depth
	ext->extGlBindRenderbuffer(GL_RENDERBUFFER, DepthSurface);
	if (MultiSample)
		ext->extGlRenderbufferStorageMultisample(GL_RENDERBUFFER, antialias * 2, GL_DEPTH_COMPONENT24_OES, size.Width, size.Height);
	else
		ext->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, size.Width, size.Height);

	ext->extGlBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (MultiSample)
		ext->extGlFramebufferTexture2DMultisample(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RTCopyTexture->getGLTexture(), 0, antialias * 2);
	else
		ext->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RTCopyTexture->getGLTexture(), 0);

	ext->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthSurface);
	
	success = ext->checkFBOStatus();
	if (!success)
	{
		ext->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
		ASSERT(false);
		return false;
	}

	ext->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);

	VideoBuilt = true;
	return true;
}

void CGLES2RenderTarget::releaseVideoTexture()
{
	CGLES2Extension* ext = static_cast<CGLES2Driver*>(g_Engine->getDriver())->getGLExtension();
	if (FrameBuffer)
	{
		ext->extGlDeleteFramebuffers(1, &FrameBuffer);
		FrameBuffer = 0;
	}
	
	if (DepthSurface)
	{
		ext->extGlDeleteRenderbuffers(1, &DepthSurface);
		DepthSurface = 0;
	}

	VideoBuilt = false;
}

#endif

#endif