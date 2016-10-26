#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "ITexture.h"

class ITextureWriter;
class IBLPImage;
class IImage;

//创建静态纹理，动态render target
class COpenGLTexture : public ITexture
{
public:
	explicit COpenGLTexture( bool mipmap );			//from blp
	~COpenGLTexture();

public:
	//ITexture
	bool createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format );
	bool createFromImage(const dimension2du& size, IImage* image);
	bool createFromBlpImage(IBLPImage* blpImage);
	bool createRTTexture( const dimension2du& size, ECOLOR_FORMAT format, int nSamples );

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

	//image
	void copyTexture( ITextureWriter* writer, IImage* image );
	bool createMipMaps( ITextureWriter* writer, u32 level = 1 );		

private:
	GLuint		GLTexture;
};


#endif