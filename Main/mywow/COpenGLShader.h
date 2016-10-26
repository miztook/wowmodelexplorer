#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "IShader.h"

class COpenGLVertexShader : public IVertexShader
{
public:
	COpenGLVertexShader(E_VS_TYPE type, GLuint vs, VSHADERCONSTCALLBACK callback);
	~COpenGLVertexShader();

public:
	bool isValid() const { return VertexShader != 0; }
	void onShaderUsed() {}

	GLuint getGLVShader() const { return VertexShader; }

private:
	GLuint		VertexShader;
};

class COpenGLPixelShader : public IPixelShader
{
public:
	COpenGLPixelShader(E_PS_TYPE type, E_PS_MACRO macro, GLuint ps, PSHADERCONSTCALLBACK callback); 
	~COpenGLPixelShader();

public:
	bool isValid() const { return PixelShader != 0; }
	void onShaderUsed() {}

	GLuint getGLPShader() const { return PixelShader; }

private:
	GLuint		PixelShader;
};

#endif