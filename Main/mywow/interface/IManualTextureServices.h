#pragma once

#include "core.h"
#include <unordered_map>
#include "ILostResetCallback.h"

class IRenderTarget;
class ITexture;
class IImage;

//手工创建的texture, 或创建render target
class IManualTextureServices : public ILostResetCallback
{
public:
	virtual ~IManualTextureServices() {}

public:
	ITexture* getManualTexture(const c8* name);
	virtual ITexture* addTexture(const c8* name, const dimension2du& size, IImage* img, bool mipmap) = 0;
	virtual void removeTexture(const c8* name) = 0;

	virtual IRenderTarget* addRenderTarget( const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt ) = 0;
	virtual void removeRenderTarget( IRenderTarget* texture ) = 0;

	virtual ITexture* createTextureFromImage(const dimension2du& size, IImage* image, bool mipmap) = 0;
	virtual ITexture* createTextureFromData(const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap) = 0;
	virtual ITexture* createCompressTextureFromData(const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap) = 0;
	virtual ITexture* createEmptyTexture(const dimension2du& size, ECOLOR_FORMAT format) = 0;

protected:
	typedef std::unordered_map<string64, ITexture*, string64::string_hash> T_TextureMap;
	T_TextureMap TextureMap;
};

inline ITexture* IManualTextureServices::getManualTexture( const c8* name )
{
	T_TextureMap::const_iterator itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return NULL_PTR;

	return (*itr).second;
}
