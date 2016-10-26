#include "stdafx.h"
#include "COpenGLHelper.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"

bool COpenGLHelper::init()
{

	return true;
}

GLint COpenGLHelper::getGLTextureEnvMode( E_TEXTURE_OP colorOp, E_TEXTURE_OP alphaOp )
{
	GLint r = GL_MODULATE;
	COpenGLExtension* extHandler = static_cast<COpenGLDriver*>(g_Engine->getDriver())->getGLExtension();

	if (extHandler->queryOpenGLFeature(IRR_ARB_texture_env_combine))
	{
		if(colorOp != ETO_DISABLE || alphaOp != ETO_DISABLE)
			r = GL_COMBINE_ARB;
	}
	else if(extHandler->queryOpenGLFeature(IRR_EXT_texture_env_combine))
	{
		if(colorOp != ETO_DISABLE || alphaOp != ETO_DISABLE)
			r = GL_COMBINE_EXT;
	}

	return r;
}

GLint COpenGLHelper::getGLTextureArg( E_TEXTURE_ARG arg )
{
	GLint r = GL_PREVIOUS;
	COpenGLExtension* extHandler = static_cast<COpenGLDriver*>(g_Engine->getDriver())->getGLExtension();

	if (extHandler->queryOpenGLFeature(IRR_ARB_texture_env_combine))
	{
		switch(arg)
		{
		case ETA_CURRENT:
			r = GL_PREVIOUS; break;
		case ETA_TEXTURE:
			r = GL_TEXTURE; break;
		case ETA_DIFFUSE:
			r = GL_PRIMARY_COLOR_ARB; break;
		default:
			ASSERT(false);
		}
	}
	else if(extHandler->queryOpenGLFeature(IRR_EXT_texture_env_combine))
	{
		switch(arg)
		{
		case ETA_CURRENT:
			r = GL_PREVIOUS; break;
		case ETA_TEXTURE:
			r = GL_TEXTURE; break;
		case ETA_DIFFUSE:
			r = GL_PRIMARY_COLOR_EXT; break;
		default:
			ASSERT(false);
		}
	}
	
	return r;
}

bool COpenGLHelper::IsGLError()
{
	bool ret = glGetError() != GL_NO_ERROR;
	ASSERT(!ret);
	return ret;
}


#endif