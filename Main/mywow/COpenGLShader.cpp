#include "stdafx.h"
#include "COpenGLShader.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"

COpenGLVertexShader::COpenGLVertexShader( E_VS_TYPE type, GLuint vs, VSHADERCONSTCALLBACK callback )
	: IVertexShader(type, callback), VertexShader(vs)
{

}

COpenGLVertexShader::~COpenGLVertexShader()
{
	COpenGLExtension*	extHandler = static_cast<COpenGLDriver*>(g_Engine->getDriver())->getGLExtension();

	extHandler->extGlDeleteObject((GLhandleARB)VertexShader);
}

COpenGLPixelShader::COpenGLPixelShader( E_PS_TYPE type, E_PS_MACRO macro, GLuint ps, PSHADERCONSTCALLBACK callback )
	: IPixelShader(type, macro, callback), PixelShader(ps)
{

}

COpenGLPixelShader::~COpenGLPixelShader()
{
	COpenGLExtension*	extHandler = static_cast<COpenGLDriver*>(g_Engine->getDriver())->getGLExtension();

	extHandler->extGlDeleteObject((GLhandleARB)PixelShader);
}

#endif


