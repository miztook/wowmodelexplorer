#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "IRenderTarget.h"

class CGLES2Texture;

#ifdef MW_PLATFORM_IOS

class CGLES2RenderTarget : public IRenderTarget
{
public:
	CGLES2RenderTarget(dimension2du size, ECOLOR_FORMAT format);
	~CGLES2RenderTarget();

public:
	virtual bool isValid() const { return FrameBuffer != 0; }
	virtual ITexture* getRTTexture() const;
	virtual bool writeToRTTexture(); 

	//lost reset
	void onLost();
	void onReset();

	 GLuint getFrameBuffer() const { return FrameBuffer; }

private:
	bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format, u32 antialias, u32 quality);
	void releaseVideoTexture();
	void bindTexture();

private:
	CGLES2Texture*		RTCopyTexture;

	GLuint		CopyFrameBuffer;

	GLuint		FrameBuffer;

	GLuint		ColorSurface;
	GLuint		DepthSurface;

	bool  MultiSample;
	u8	Padding[3];
};

#else

class CGLES2RenderTarget : public IRenderTarget
{
public:
	CGLES2RenderTarget(dimension2du size, ECOLOR_FORMAT format);
	~CGLES2RenderTarget();

public:
	virtual bool isValid() const { return FrameBuffer != NULL; }
	virtual ITexture* getRTTexture() const;
	virtual bool writeToRTTexture(); 

	//lost reset
	void onLost();
	void onReset();

	GLuint getFrameBuffer() const { return FrameBuffer; }

private:
	bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format, u32 antialias, u32 quality);
	void releaseVideoTexture();

private:
	CGLES2Texture*		RTCopyTexture;

	GLuint		FrameBuffer;
	GLuint		DepthSurface;

	bool  MultiSample;
	u8	Padding[3];
};

#endif

#endif