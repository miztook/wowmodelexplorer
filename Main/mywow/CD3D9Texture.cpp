#include "stdafx.h"
#include "CD3D9Texture.h"
#include "mywow.h"
#include "compileconfig.h"


#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "CBlit.h"

CD3D9Texture::CD3D9Texture( bool mipmap )
  : ITexture(mipmap), DXTexture(nullptr)
{
}

CD3D9Texture::~CD3D9Texture()
{
	releaseVideoResources();
}

bool CD3D9Texture::buildVideoResources()
{
	//CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	IBLPImage* blpImage = g_Engine->getResourceLoader()->loadBLP(getFileName());
	if(!blpImage || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? blpImage->getNumMipLevels() : 1;
	TextureSize = blpImage->getDimension();
	ColorFormat = blpImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, NumMipmaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9Texture::createVideoTexture Failed");
		
		blpImage->drop();

		ASSERT(false);
		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, blpImage);

	if (HasMipMaps)
		copyBlpMipMaps(texWriter, blpImage);

	blpImage->drop();

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

void CD3D9Texture::releaseVideoResources()
{
	if (VideoBuilt)
	{
		//CLock lock(&g_Globals.textureCS);
		SAFE_RELEASE(DXTexture);
		VideoBuilt = false;
	}
}

bool CD3D9Texture::createEmptyTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	ASSERT(!HasMipMaps);

	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;

	if (!createTexture(TextureSize, ColorFormat, NumMipmaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9Texture::createEmptyTexture Failed");
		ASSERT(false);
		return false;
	}

	if (!HasMipMaps)
	{
		ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);
		texWriter->initEmptyData();
		texWriter->copyToTexture(this);
		g_Engine->getTextureWriteServices()->removeTextureWriter(this);
	}

	VideoBuilt = true;

	return true;
}

bool CD3D9Texture::createRTTexture( const dimension2du& size, ECOLOR_FORMAT format )
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	ASSERT(!HasMipMaps);

	Type = ETT_RENDERTARGET;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;

	D3DFORMAT d3dfmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	IDirect3DDevice9* device = (IDirect3DDevice9*)driver->pID3DDevice;
	IDirect3DTexture9* tex = nullptr;
	HRESULT hr = device->CreateTexture(TextureSize.Width, TextureSize.Height,
		1,			//no mipmap
		D3DUSAGE_RENDERTARGET,			//no lock
		d3dfmt,
		D3DPOOL_DEFAULT,
		&tex,
		nullptr);

	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9Texture::createRTTexture Failed");
		ASSERT(false);
		return false;
	}

	DXTexture = tex;

	VideoBuilt = true;

	return true;
}

bool CD3D9Texture::createDSTexture( const dimension2du& size )
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}
	
	ASSERT(!HasMipMaps);

	Type = ETT_DEPTHSTENCIL;

	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	IDirect3DDevice9* device = (IDirect3DDevice9*)driver->pID3DDevice;

	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = ECF_UNKNOWN;

	D3DFORMAT d3dfmt = D3DFMT_D24X8;
	if (driver->DefaultDepthBuffer)
	{
		D3DSURFACE_DESC desc;
		driver->DefaultDepthBuffer->GetDesc(&desc);
		d3dfmt = desc.Format;
	}

	IDirect3DTexture9* tex = nullptr;
	HRESULT hr = device->CreateTexture(TextureSize.Width, TextureSize.Height,
		1,			//no mipmap
		D3DUSAGE_DEPTHSTENCIL,			//no lock
		d3dfmt,
		D3DPOOL_DEFAULT,
		&tex,
		nullptr);

	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9Texture::createDSTexture Failed");
		ASSERT(false);
		return false;
	}

	DXTexture = tex;

	VideoBuilt = true;

	return true;
}

bool CD3D9Texture::createFromImage( const dimension2du& size, IImage* image )
{
	//CLock lock(&g_Globals.textureCS);

	if(VideoBuilt || !image || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? image->getDimension().getNumMipLevels() : 1;
	TextureSize =  size;	//image->getDimension();
	ColorFormat = image->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, NumMipmaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9Texture::createFromImage Failed");
		ASSERT(false);
		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, image);

	if (HasMipMaps)
		createMipMaps(texWriter);				//填充mipmap

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

bool CD3D9Texture::createFromBlpImage( IBLPImage* blpImage )
{
	//CLock lock(&g_Globals.textureCS);
	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	if(!blpImage || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? blpImage->getNumMipLevels() : 1;
	TextureSize = blpImage->getDimension();
	ColorFormat = blpImage->getColorFormat();

	if (!createTexture(TextureSize, ColorFormat, NumMipmaps))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9Texture::createFromBlpImage Failed");
		ASSERT(false);

		return false;
	}

	ITextureWriter* texWriter = g_Engine->getTextureWriteServices()->createTextureWriter(this, true);

	copyTexture(texWriter, blpImage);

	if (HasMipMaps)
		copyBlpMipMaps(texWriter, blpImage);

	texWriter->copyToTexture(this);

	g_Engine->getTextureWriteServices()->removeTextureWriter(this);

	VideoBuilt = true;

	return true;
}

bool CD3D9Texture::createTexture( const dimension2du& size, ECOLOR_FORMAT format, uint32_t numMipmap )
{
	if(VideoBuilt || DXTexture)
	{
		ASSERT(false);
		return false;
	}

	D3DFORMAT d3dfmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	IDirect3DDevice9* device = (IDirect3DDevice9*)driver->pID3DDevice;
	HRESULT hr = device->CreateTexture(TextureSize.Width, TextureSize.Height,
		numMipmap, // number of mipmaplevels (0 = automatic all)
		0,		//
		d3dfmt, 
		g_Engine->getOSInfo()->IsAeroSupport() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
		&DXTexture, nullptr);

	if (FAILED(hr))
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CD3D9Texture::createTexture Failed");
		ASSERT(false);
		return false;
	}

	return true;
}

void CD3D9Texture::copyTexture( ITextureWriter* writer, IBLPImage* blpimage )
{
	ASSERT(blpimage);
	
	uint32_t pitch;
	void* destData = writer->lock(0, pitch);

	bool result = blpimage->copyMipmapData(0, destData, pitch, TextureSize.Width, TextureSize.Height);
	ASSERT(result);

	writer->unlock(0);
}

bool CD3D9Texture::copyBlpMipMaps( ITextureWriter* writer, IBLPImage* blpimage, uint32_t level /*= 1*/ )
{
	if (!HasMipMaps || !blpimage)
		return false;

	if (level==0 || level >= NumMipmaps)
		return true;

	dimension2du size = TextureSize.getMipLevelSize(level);

	//lock
	uint32_t pitch;
	void* destData = writer->lock(level, pitch);

	bool result = blpimage->copyMipmapData(level, destData, pitch, size.Width, size.Height);

	// unlock
	writer->unlock(level);

	if (!result || (size.Width < 2 && size.Height < 2))
		return false; // stop generating levels

	// generate next level
	return copyBlpMipMaps(writer, blpimage, level+1);
}

void CD3D9Texture::copyTexture( ITextureWriter* writer, IImage* image )
{
	ASSERT(image);

	uint32_t pitch;
	void* destData = writer->lock(0, pitch);

	image->copyToScaling(destData, TextureSize.Width, TextureSize.Height, ColorFormat, pitch);

	writer->unlock(0);
}

bool CD3D9Texture::createMipMaps( ITextureWriter* writer, uint32_t level /*= 1 */ )
{
	ASSERT(HasMipMaps);

	if (level==0 || level >= NumMipmaps)
		return true;

	dimension2du upperSize = TextureSize.getMipLevelSize(level - 1);
	dimension2du lowerSize = TextureSize.getMipLevelSize(level);

	// lock upper surface
	uint32_t upperPitch, lowerPitch;
	const uint8_t* 	upperDest = (const uint8_t*)writer->lock(level - 1, upperPitch);
	uint8_t*	lowerDest = (uint8_t*)writer->lock(level, lowerPitch);

	if (ColorFormat == ECF_R5G6B5 || ColorFormat == ECF_A1R5G5B5)
		CBlit::copy16BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch, ColorFormat);
	else if (ColorFormat == ECF_A8R8G8B8)
		CBlit::copy32BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch, ColorFormat);
	else if (ColorFormat == ECF_R8G8B8)
		CBlit::copyR8G8B8BitMipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch);
	else if (ColorFormat == ECF_A8L8)
		CBlit::copyA8L8MipMap(upperDest, lowerDest,
		lowerSize.Width, lowerSize.Height,
		upperPitch, lowerPitch);
	else
		ASSERT(false);				//不支持的格式
	
	writer->unlock(level);
	writer->unlock(level - 1);

	if (lowerSize.Width < 2 && lowerSize.Height < 2)
		return false; // stop generating levels

	// generate next level
	return createMipMaps(writer, level+1);
}

#endif

