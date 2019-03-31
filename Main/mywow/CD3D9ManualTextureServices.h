#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IManualTextureServices.h"
#include <map>
#include <list>
#include "fixstring.h"

class CD3D9Driver;

class CD3D9ManualTextureServices : public IManualTextureServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9ManualTextureServices);

public:
	CD3D9ManualTextureServices();
	~CD3D9ManualTextureServices();

public:
	virtual ITexture* addTexture(const char* name, const dimension2du& size, IImage* img, bool mipmap);
	virtual void removeTexture(const char* name);

	virtual IRenderTarget* addRenderTarget( const dimension2du& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt );
	virtual void removeRenderTarget( IRenderTarget* texture );

	virtual ITexture* createTextureFromImage(const dimension2du& size, IImage* image, bool mipmap);
	virtual ITexture* createTextureFromData(const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap);
	virtual ITexture* createCompressTextureFromData(const dimension2du& size, ECOLOR_FORMAT format, const void* data, bool mipmap);
	virtual ITexture* createEmptyTexture(const dimension2du& size, ECOLOR_FORMAT format);

private:
	void loadDefaultTextures();

private:
	typedef std::list<IRenderTarget*, qzone_allocator<IRenderTarget*> >	T_RTList;
	T_RTList		RenderTargets;

	CD3D9Driver* Driver;
};

#endif