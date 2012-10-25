#pragma once

#include "IManualTextureServices.h"
#include <map>
#include <list>
#include "str.h"

class CD3D9ManualTextureServices : public IManualTextureServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9ManualTextureServices);

public:
	CD3D9ManualTextureServices();
	~CD3D9ManualTextureServices();

public:
	virtual ITexture* getManualTexture(const c8* name);

	virtual ITexture* addTexture( const c8* name , IImage* img);
	virtual void removeTexture(const c8* name);

	virtual IRenderTarget* addRenderTarget( dimension2du size, ECOLOR_FORMAT format );
	virtual void removeRenderTarget( IRenderTarget* texture );

	virtual ITexture* createTextureFromData(dimension2du size, ECOLOR_FORMAT format, void* data, bool mipmap);
	virtual ITexture* createTextureFromImage(IImage* image, bool mipmap);
	virtual ITexture* createEmptyTexture(dimension2du size, ECOLOR_FORMAT format);

public:
	virtual void onLost();
	virtual void onReset();

private:
	
	typedef std::map<string64, ITexture*, std::less<string64>, qzone_allocator<std::pair<string64, ITexture*>>> T_TextureMap;
	T_TextureMap TextureMap;

	typedef std::list<IRenderTarget*, qzone_allocator<ITexture*>>	T_RTList;
	T_RTList		RenderTargets;
};