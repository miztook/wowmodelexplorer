#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "ITexture.h"

class ITextureWriter;
class IBLPImage;
class IPVRImage;
class IKTXImage;
class IImage;

//创建静态纹理，动态render target
class CGLES2Texture : public ITexture
{
public:
	explicit CGLES2Texture( bool mipmap );			//from blp
	~CGLES2Texture();

public:
	//ITexture
	bool createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format );
	bool createFromImage(const dimension2du& size, IImage* image);
	bool createFromBlpImage(IBLPImage* blpImage);
	bool createFromPvrImage(IPVRImage* pvrImage);
	bool createFromKtxImage(IKTXImage* ktxImage);
	bool createRTTexture( const dimension2du& size, ECOLOR_FORMAT format );
	bool createDSTexture( const dimension2du& size );

	virtual bool isValid() const { return GLTexture != 0; }

	//video memory
	virtual bool createVideoTexture();
	virtual void releaseVideoTexture();

	 GLuint getGLTexture() const { return GLTexture; }

private:
	bool createTexture( const dimension2du& size, ECOLOR_FORMAT format, bool mipmap = true );

	//blp
	void copyTexture( ITextureWriter* writer, IBLPImage* blpimage );
	bool copyBlpMipMaps( ITextureWriter* writer, IBLPImage* blpimage, u32 level = 1);

	//pvr
	void copyTexture( ITextureWriter* writer, IPVRImage* pvrimage );
	bool copyPvrMipMaps( ITextureWriter* writer, IPVRImage* pvrimage, u32 level = 1);

	//ktx
	void copyTexture( ITextureWriter* writer, IKTXImage* ktxImage );
	bool copyKtxMipMaps( ITextureWriter* writer, IKTXImage* ktxImage, u32 level = 1);

	//image
	void copyTexture( ITextureWriter* writer, IImage* image );
	bool createMipMaps( ITextureWriter* writer, u32 level = 1 );		

private:
	GLuint		GLTexture;
};

#endif