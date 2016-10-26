#pragma once

#include "ISpecialTextureServices.h"
#include <map>

class CSpecialTextureServices : public ISpecialTextureServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CSpecialTextureServices);

public:
	CSpecialTextureServices();
	~CSpecialTextureServices();

public:
	virtual ITexture* getTexture(E_SPECIAL_TEXTURES tex) const;

private:
	void loadTextures();

private:
	ITexture* Textures[EST_TEXTURE_COUNT];
};