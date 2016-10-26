#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CResourceLoader.h"

class CGLES2ResourceLoader : public CResourceLoader
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2ResourceLoader);

public:
	CGLES2ResourceLoader();

public:
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true);
};

#endif