#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "IRenderTarget.h"

class COpenGLTexture;

class COpenGLRenderTarget : public IRenderTarget
{
public:
	COpenGLRenderTarget(const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt);
	~COpenGLRenderTarget();

public:
	virtual bool isValid() const { return FrameBuffer != 0; }
	virtual ITexture* getRTTexture() const;
	virtual bool writeToRTTexture(); 

	//lost reset
	void onLost();
	void onReset();

	GLuint getFrameBuffer() const { return FrameBuffer; }

private:
	bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, u8 antialias);
	void releaseVideoTexture();
	void bindTexture();

private:
	COpenGLTexture*		RTCopyTexture;
	GLuint		CopyFrameBuffer;
	
	GLuint		FrameBuffer;
	COpenGLTexture*		ColorTexture;
	GLuint		DepthSurface;

	bool  MultiSample;
};


#endif