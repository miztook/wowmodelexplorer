#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CResourceLoader.h"

class CD3D9ResourceLoader : public CResourceLoader
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9ResourceLoader);

public:
	CD3D9ResourceLoader();

public:
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true);
};

#endif