#pragma once

#include "core.h"

class ITexture;

enum E_SPECIAL_TEXTURES
{
	EST_ARMORREFLECT = 0,
	EST_ARMORREFLECT_RAINBOW,

	EST_TEXTURE_COUNT
};

//特殊用途的纹理
class ISpecialTextureServices
{
public:
	virtual ~ISpecialTextureServices() {}

public:
	virtual ITexture* getTexture(E_SPECIAL_TEXTURES tex) const = 0;
};