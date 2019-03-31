#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "ITexture.h"

class ITextureWriter;
class IBLPImage;
class IImage;

class CD3D11Texture : public ITexture
{
public:
	explicit CD3D11Texture( bool mipmap );
	~CD3D11Texture();

public:
	//ITexture
	bool createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format );
	bool createFromImage(const dimension2du& size, IImage* image);
	bool createFromBlpImage(IBLPImage* blpImage);
	bool createRTTexture( const dimension2du& size, ECOLOR_FORMAT format, uint32_t antialias, uint32_t quality );
	bool createDSTexture( const dimension2du& size, ECOLOR_FORMAT format, uint32_t antialias, uint32_t quality );

	virtual bool isValid() const { return DXTexture!=nullptr; }

	ID3D11Texture2D* getDXTexture() const { return DXTexture; }
	ID3D11ShaderResourceView* getShaderResourceView() const { return SRView; }

protected:
	virtual bool buildVideoResources();
	virtual void releaseVideoResources();
	virtual bool hasVideoBuilt() const { return VideoBuilt; }

private:
	bool createViews(DXGI_FORMAT format, uint32_t mipmapLevels, bool multisample);
	bool createTexture( const dimension2du& size, IImage* image, bool mipmap );
	bool createTexture( IBLPImage* blpimage, bool mipmap );

	void buildTempSysMemData(D3D11_SUBRESOURCE_DATA* subData, uint32_t size, IBLPImage* blpimage);
	void buildTempSysMemData(D3D11_SUBRESOURCE_DATA* subData, uint32_t size, IImage* image);

	void destroyTempSysMemData(D3D11_SUBRESOURCE_DATA* subData, uint32_t size);

private:
	ID3D11Texture2D*		DXTexture;
	ID3D11ShaderResourceView*	SRView;
};

#endif