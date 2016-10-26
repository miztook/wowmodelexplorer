#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "ITexture.h"

class ITextureWriter;
class IBLPImage;
class IImage;

//创建静态纹理，动态render target
class CD3D9Texture : public ITexture
{
public:
	explicit CD3D9Texture( bool mipmap );			//from blp
	~CD3D9Texture();

public:
	//ITexture
	bool createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format );
	bool createFromImage( const dimension2du& size, IImage* image);
	bool createFromBlpImage(IBLPImage* blpImage);
	bool createRTTexture( const dimension2du& size, ECOLOR_FORMAT format );
	bool createDSTexture( const dimension2du& size );

	virtual bool isValid() const { return DXTexture!=NULL_PTR; }

	//video memory
	virtual bool createVideoTexture();
	virtual void releaseVideoTexture();

	//
	 IDirect3DTexture9*  getDXTexture() const { return DXTexture; }

private:
	bool createTexture( const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap );
	
	//blp
	void copyTexture( ITextureWriter* writer, IBLPImage* blpimage );
	bool copyBlpMipMaps( ITextureWriter* writer, IBLPImage* blpimage, u32 level = 1);

	//image
	void copyTexture( ITextureWriter* writer, IImage* image );
	bool createMipMaps( ITextureWriter* writer, u32 level = 1 );		

private:
	IDirect3DTexture9*		DXTexture;
};

#endif