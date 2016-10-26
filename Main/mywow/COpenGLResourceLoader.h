#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "CResourceLoader.h"

class COpenGLResourceLoader : public CResourceLoader
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLResourceLoader);

public:
	COpenGLResourceLoader();

public:
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true);
};

#endif