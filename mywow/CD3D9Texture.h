#pragma once

#include "base.h"
#include "ITexture.h"

class IImage;
class CD3D9Driver;

//创建静态纹理，动态render target
class CD3D9Texture : public ITexture
{
public:
	CD3D9Texture();
	~CD3D9Texture();

	CD3D9Texture( IImage* image, bool mipmap );		//from image format

public:
	//ITexture
	virtual bool createAsRenderTarget( dimension2du size, ECOLOR_FORMAT format );
	virtual bool createEmptyTexture( dimension2du size, ECOLOR_FORMAT format, bool mipmap );
	virtual void* lock(bool readOnly = false, u32 mipmapLevel=0);
	virtual void unlock();

	virtual dimension2du getOriginalSize() const { return ImageSize; }
	virtual dimension2du getSize() const { return TextureSize; }
	virtual ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }
	virtual u32 getPitch() const { return Pitch; }
	virtual bool hasMipMaps() const { return HasMipMaps; }
	virtual bool isRenderTarget() const { return IsRenderTarget; }

	virtual bool isValid() const { return DXTexture!=NULL; }

	virtual bool createFromImage();
	virtual bool createMipMaps( u32 level = 1 );				//自动生成mipmap			
	virtual void releaseTexture();

	//lost reset
	void onLost();
	void onReset();

	//
	IDirect3DTexture9*  getDXTexture() const { return DXTexture; }
	IDirect3DSurface9*	getRTTSurface() const { return RTTSurface; }
	IDirect3DSurface9* getRTTDepthSurface() const { return DepthSurface; }

private:
	bool createFromDXTexture( IDirect3DTexture9* d3d9Texture );

	bool createTexture( IImage* image, u32 maxTextureSize, bool mipmap = true );

	void copyTexture( IImage* image );

	void copyA8L8MipMap(char* src, char* tgt,
		s32 width, s32 height,  s32 pitchsrc, s32 pitchtgt) const;

	void copy16BitMipMap(char* src, char* tgt,
		s32 width, s32 height,  s32 pitchsrc, s32 pitchtgt) const;

	void copy32BitMipMap(char* src, char* tgt,
		s32 width, s32 height,  s32 pitchsrc, s32 pitchtgt) const;

	void setPitch(D3DFORMAT d3dformat);

private:
	IDirect3DTexture9*		DXTexture;
	IDirect3DSurface9*		RTTSurface;
	IDirect3DSurface9*		DepthSurface;

	dimension2du	TextureSize;
	dimension2du	ImageSize;
	s32		Pitch;
	u32		MipLevelLocked;
	ECOLOR_FORMAT	ColorFormat;

	bool	HasMipMaps;
	bool	IsRenderTarget;

	IImage*		Image;

	//
	CD3D9Driver*	Driver;
	IDirect3DDevice9*	Device;

};