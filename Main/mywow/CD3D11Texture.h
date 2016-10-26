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
	bool createRTTexture( const dimension2du& size, ECOLOR_FORMAT format, u32 antialias, u32 quality );
	bool createDSTexture( const dimension2du& size, ECOLOR_FORMAT format, u32 antialias, u32 quality );

	virtual bool isValid() const { return DXTexture!=NULL_PTR; }

	//video memory
	virtual bool createVideoTexture();
	virtual void releaseVideoTexture();

	ID3D11Texture2D* getDXTexture() const { return DXTexture; }
	ID3D11ShaderResourceView* getShaderResourceView() const { return SRView; }

private:
	bool createViews(DXGI_FORMAT format, u32 mipmapLevels, bool multisample);
	bool createTexture( const dimension2du& size, IImage* image, bool mipmap );
	bool createTexture( IBLPImage* blpimage, bool mipmap );

	void buildTempSysMemData(D3D11_SUBRESOURCE_DATA* subData, u32 size, IBLPImage* blpimage);
	void buildTempSysMemData(D3D11_SUBRESOURCE_DATA* subData, u32 size, IImage* image);

	void destroyTempSysMemData(D3D11_SUBRESOURCE_DATA* subData, u32 size);

private:
	ID3D11Texture2D*		DXTexture;
	ID3D11ShaderResourceView*	SRView;
};

#endif