#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "IManualTextureServices.h"

class CD3D11Driver;

class CD3D11ManualTextureServices : public  IManualTextureServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11ManualTextureServices);

public:
	CD3D11ManualTextureServices();
	~CD3D11ManualTextureServices();

public:
	virtual ITexture* addTexture( const char* name, const dimension2du& size, IImage* img, bool mipmap);
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
	typedef std::list<IRenderTarget*, qzone_allocator<ITexture*> >	T_RTList;
	T_RTList		RenderTargets;

	CD3D11Driver*	Driver;
};

#endif