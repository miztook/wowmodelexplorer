#include "stdafx.h"
#include "CD3D9ResourceLoader.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Texture.h"

CD3D9ResourceLoader::CD3D9ResourceLoader()
{

}

ITexture* CD3D9ResourceLoader::loadTexture( const c8* filename, bool mipmap /*= true*/ )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&textureCS);

	ITexture* tex  = TextureCache.tryLoadFromCache(realfilename);
	if (tex)
	{
		if (MultiThread)
			END_LOCK(&textureCS);

		return tex;
	}

	if (g_Engine->getWowEnvironment()->exists(realfilename))
	{
		tex = new CD3D9Texture(mipmap);
		tex->setFileName(realfilename);
		TextureCache.addToCache(tex);
	}

	if (MultiThread)
		END_LOCK(&textureCS);

	return tex;
}

#endif