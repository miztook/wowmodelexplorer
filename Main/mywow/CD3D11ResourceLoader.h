#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CResourceLoader.h"

class CD3D11ResourceLoader : public CResourceLoader
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11ResourceLoader);

public:
	CD3D11ResourceLoader();

public:
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true);
};

#endif