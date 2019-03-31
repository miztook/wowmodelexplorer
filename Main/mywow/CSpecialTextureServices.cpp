#include "stdafx.h"
#include "CSpecialTextureServices.h"
#include "mywow.h"

static const char* texArmorReflect = "item\\objectcomponents\\shield\\armorreflect4.blp";
static const char* texArmorReflectRainbow = "item\\objectcomponents\\weapon\\armorreflect_rainbow.blp";

CSpecialTextureServices::CSpecialTextureServices()
{
	memset(Textures, 0, sizeof(Textures));

	loadTextures();
}

CSpecialTextureServices::~CSpecialTextureServices()
{
	for (uint32_t i=0; i<EST_TEXTURE_COUNT; ++i)
	{
		if (Textures[i])
			Textures[i]->drop();
	}
}

ITexture* CSpecialTextureServices::getTexture( E_SPECIAL_TEXTURES tex ) const
{
	int32_t idx = (int32_t)tex;
	if (idx >= EST_TEXTURE_COUNT)
		return nullptr;

	return Textures[idx];
}

void CSpecialTextureServices::loadTextures()
{
	ITexture* tex = g_Engine->getResourceLoader()->loadTexture(texArmorReflect, true);
	if (tex)
	{
		IVideoResource::buildVideoResources(tex);
		Textures[EST_ARMORREFLECT] = tex;
	}

	tex = g_Engine->getResourceLoader()->loadTexture(texArmorReflectRainbow, true);
	if (tex)
	{
		IVideoResource::buildVideoResources(tex);
		Textures[EST_ARMORREFLECT_RAINBOW] = tex;
	}
}
