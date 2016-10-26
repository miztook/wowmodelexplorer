#include "stdafx.h"
#include "CGLES2ResourceLoader.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Texture.h"
#include "CSysSync.h"

CGLES2ResourceLoader::CGLES2ResourceLoader()
{

}

ITexture* CGLES2ResourceLoader::loadTexture( const c8* filename, bool mipmap /*= true*/ )
{
	if (strlen(filename) == 0)
		return NULL;

	string256 path = filename;

#ifdef MW_COMPILE_WITH_GLES2
#if defined(USE_PVR)
	path.changeExt(".blp", ".pvr");
#elif defined(USE_KTX)
	path.changeExt(".blp", ".ktx");
#endif
#endif

	path.normalize();
	path.make_lower();

	if (MultiThread)
		BEGIN_LOCK(&textureCS);

	ITexture* tex  = TextureCache.tryLoadFromCache(path.c_str());
	if (tex)
	{
		if (MultiThread)
			END_LOCK(&textureCS);

		return tex;
	}

	if (g_Engine->getWowEnvironment()->exists(path.c_str()))
	{
		tex = new CGLES2Texture(mipmap);
		tex->setFileName(path.c_str());
		TextureCache.addToCache(tex);
	}

	if (MultiThread)
		END_LOCK(&textureCS);

	return tex;
}

#endif