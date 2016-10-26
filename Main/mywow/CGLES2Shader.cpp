#include "stdafx.h"
#include "CGLES2Shader.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"

CGLES2VertexShader::CGLES2VertexShader( E_VS_TYPE type, GLuint vs, VSHADERCONSTCALLBACK callback )
	: IVertexShader(type, callback), VertexShader(vs)
{

}

CGLES2VertexShader::~CGLES2VertexShader()
{
	CGLES2Extension*	extHandler = static_cast<CGLES2Driver*>(g_Engine->getDriver())->getGLExtension();

	extHandler->extGlDeleteShader(VertexShader);
}

CGLES2PixelShader::CGLES2PixelShader( E_PS_TYPE type, E_PS_MACRO macro, GLuint ps, PSHADERCONSTCALLBACK callback )
	: IPixelShader(type, macro, callback), PixelShader(ps)
{

}

CGLES2PixelShader::~CGLES2PixelShader()
{
	CGLES2Extension*	extHandler = static_cast<CGLES2Driver*>(g_Engine->getDriver())->getGLExtension();

	extHandler->extGlDeleteShader(PixelShader);
}

#endif