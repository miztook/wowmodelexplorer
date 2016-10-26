#include "stdafx.h"
#include "COpenGLRenderTarget.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLTexture.h"
#include "COpenGLHelper.h"
#include "COpenGLMaterialRenderServices.h"

COpenGLRenderTarget::COpenGLRenderTarget( const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt )
	: IRenderTarget(colorFmt, depthFmt), FrameBuffer(0), CopyFrameBuffer(0), ColorTexture(NULL_PTR), DepthSurface(0), MultiSample(false)
{
	RTCopyTexture = new COpenGLTexture(false);
	
	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(size, colorFmt, depthFmt, setting.antialias);
	ASSERT(success);
	
	if (MultiSample)
	{
		success = RTCopyTexture->createEmptyTexture(TextureSize, ColorFormat);
		ASSERT(success);
		bindTexture();
	}
}

COpenGLRenderTarget::~COpenGLRenderTarget()
{
	releaseVideoTexture();	
	RTCopyTexture->drop();
}

ITexture* COpenGLRenderTarget::getRTTexture() const
{
	return MultiSample ? RTCopyTexture : ColorTexture;
}

bool COpenGLRenderTarget::writeToRTTexture()
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLExtension* ext = driver->getGLExtension();
	if (MultiSample)
	{
		ASSERT(CopyFrameBuffer);
		ext->extGlBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, FrameBuffer);
		ext->extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, CopyFrameBuffer);
		ext->extGlBlitFramebufferEXT(0, 0, TextureSize.Width, TextureSize.Height, 0, 0, TextureSize.Width, TextureSize.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		if (ext->DiscardFrameSupported)
		{
			const GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT };
			ext->extGlInvalidateFramebuffer(GL_READ_FRAMEBUFFER_EXT, 2, attachments);
		}
		ext->extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, 0);
		ext->extGlBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, 0);
	}
	
	return true;
}

void COpenGLRenderTarget::onLost()
{
	if (MultiSample)
		RTCopyTexture->releaseVideoTexture();

	releaseVideoTexture();
}

void COpenGLRenderTarget::onReset()
{
	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();
	bool success = createAsRenderTarget(TextureSize, ColorFormat, DepthFormat, setting.antialias);
	ASSERT(success);

	if (MultiSample)
	{
		success = RTCopyTexture->createRTTexture(TextureSize, ColorFormat, 0);
		ASSERT(success);
		bindTexture();
	}
}

bool COpenGLRenderTarget::createAsRenderTarget( dimension2du size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, u8 antialias )
{
	if (VideoBuilt)
		return false;

	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLExtension* ext = driver->getGLExtension();

	MultiSample = ext->MultisampleSupported && antialias > 0;

	TextureSize = size;
	// 		size.getOptimalSize(!driver->queryFeature(EVDF_TEXTURE_NPOT), 
	// 		!driver->queryFeature(EVDF_TEXTURE_NSQUARE), 
	// 		true, 
	// 		driver->getGLExtension()->MaxTextureSize);

	ColorFormat = colorFmt;

	if(ColorFormat == ECF_UNKNOWN)
	{
		ColorFormat = driver->ColorFormat;
	}

	DepthFormat = depthFmt;

	if (DepthFormat == ECF_UNKNOWN)
	{
		DepthFormat = driver->DepthFormat;
	}

	//
	ASSERT(!FrameBuffer && !ColorTexture && !DepthSurface && !CopyFrameBuffer);

	ext->extGlGenFramebuffers(1, &FrameBuffer);
	ext->extGlGenRenderbuffers(1, &DepthSurface);

	ext->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, FrameBuffer);

	if (MultiSample)
		ext->extGlGenFramebuffers(1, &CopyFrameBuffer);

	ASSERT(ext->MaxMultiSample >= antialias * 2);

	//color
	ColorTexture = new COpenGLTexture(false);
	if(MultiSample)
		ColorTexture->createRTTexture(TextureSize, ColorFormat, antialias * 2);
	else
		ColorTexture->createRTTexture(TextureSize, ColorFormat, 0);

	//depth
	GLenum dfmt = COpenGLHelper::getGLDepthFormat(DepthFormat);
	ext->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, DepthSurface);
	if (MultiSample)
		ext->extGlRenderbufferStorageMultisample(GL_RENDERBUFFER_EXT, antialias * 2, dfmt, size.Width, size.Height);
	else
		ext->extGlRenderbufferStorage(GL_RENDERBUFFER_EXT, dfmt, size.Width, size.Height);
	ext->extGlBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);

	ext->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, MultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, ColorTexture->getGLTexture(), 0);
	ext->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER_EXT, DepthSurface);

	bool success = ext->checkFBOStatus();
	if (!success)
	{
		ext->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
		ASSERT(false);
		return false;
	}

	ext->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);

	VideoBuilt = true;
	return true;
}

void COpenGLRenderTarget::releaseVideoTexture()
{
	COpenGLExtension* ext = static_cast<COpenGLDriver*>(g_Engine->getDriver())->getGLExtension();
	if (DepthSurface)
	{
		ext->extGlDeleteRenderbuffers(1, &DepthSurface);
		DepthSurface = 0;
	}

	if (ColorTexture)
	{
		ColorTexture->drop();
		ColorTexture = NULL_PTR;
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

void COpenGLRenderTarget::bindTexture()
{
	if (MultiSample)
	{
		COpenGLExtension* ext = static_cast<COpenGLDriver*>(g_Engine->getDriver())->getGLExtension();

		ext->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, CopyFrameBuffer);
		ext->extGlFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, RTCopyTexture->getGLTexture(), 0);
		
		bool success = ext->checkFBOStatus();
		ASSERT(success);

		ext->extGlBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	}
}

#endif

