#include "stdafx.h"
#include "CSpecialTextureServices.h"
#include "mywow.h"

static const c8* texArmorReflect = "item\\objectcomponents\\shield\\armorreflect4.blp";
static const c8* texArmorReflectRainbow = "item\\objectcomponents\\weapon\\armorreflect_rainbow.blp";

CSpecialTextureServices::CSpecialTextureServices()
{
	memset(Textures, 0, sizeof(Textures));

	loadTextures();
}

CSpecialTextureServices::~CSpecialTextureServices()
{
	for (u32 i=0; i<EST_TEXTURE_COUNT; ++i)
	{
		if (Textures[i])
			Textures[i]->drop();
	}
}

ITexture* CSpecialTextureServices::getTexture( E_SPECIAL_TEXTURES tex ) const
{
	s32 idx = (s32)tex;
	if (idx >= EST_TEXTURE_COUNT)
		return NULL_PTR;

	return Textures[idx];
}

void CSpecialTextureServices::loadTextures()
{
	ITexture* tex = g_Engine->getResourceLoader()->loadTexture(texArmorReflect, true);
	if (tex)
	{
		tex->createVideoTexture();
		Textures[EST_ARMORREFLECT] = tex;
	}

	tex = g_Engine->getResourceLoader()->loadTexture(texArmorReflectRainbow, true);
	if (tex)
	{
		tex->createVideoTexture();
		Textures[EST_ARMORREFLECT_RAINBOW] = tex;
	}
}
