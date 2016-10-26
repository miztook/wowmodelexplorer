#include "stdafx.h"
#include "CGLES2Helper.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"

bool CGLES2Helper::init()
{

	return true;
}

bool CGLES2Helper::IsGLError()
{
	bool ret = glGetError() != GL_NO_ERROR;
	ASSERT(!ret);
	return ret;
}

#endif