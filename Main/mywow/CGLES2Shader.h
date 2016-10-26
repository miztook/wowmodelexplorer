#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "IShader.h"

class CGLES2VertexShader : public IVertexShader
{
public:
	CGLES2VertexShader(E_VS_TYPE type, GLuint vs, VSHADERCONSTCALLBACK callback);
	~CGLES2VertexShader();

public:
	bool isValid() const { return VertexShader != 0; }
	void onShaderUsed() {}

	GLuint getGLVShader() const { return VertexShader; }

private:
	GLuint		VertexShader;
};

class CGLES2PixelShader : public IPixelShader
{
public:
	CGLES2PixelShader(E_PS_TYPE type, E_PS_MACRO macro, GLuint ps, PSHADERCONSTCALLBACK callback); 
	~CGLES2PixelShader();

public:
	bool isValid() const { return PixelShader != 0; }
	void onShaderUsed() {}

	GLuint getGLPShader() const { return PixelShader; }

private:
	GLuint		PixelShader;

};

#endif