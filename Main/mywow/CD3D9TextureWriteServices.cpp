#include "stdafx.h"
#include "CD3D9TextureWriteServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Driver.h"
#include "CD3D9Helper.h"
#include "CD3D9Texture.h"

CD3D9TextureWriter::CD3D9TextureWriter(const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap)
	: ITextureWriter(numMipmap), NeedCreate(true)
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	TextureSize = size;
	ColorFormat = format;

	D3DFORMAT fmt = CD3D9Helper::getD3DFormatFromColorFormat(format);

	if(FAILED(Driver->pID3DDevice->CreateTexture(TextureSize.Width, TextureSize.Height,
		NumMipmaps,
		0,
		fmt, 
		D3DPOOL_SYSTEMMEM, 
		&SrcTexture, 
		NULL_PTR)))
	{
		ASSERT(false);
		SrcTexture = NULL_PTR;
	}
}

CD3D9TextureWriter::CD3D9TextureWriter() 
	: SrcTexture(NULL_PTR), NeedCreate(false), ITextureWriter(1)
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
}

CD3D9TextureWriter::~CD3D9TextureWriter()
{
	if (NeedCreate)
		SAFE_RELEASE(SrcTexture);
}

void CD3D9TextureWriter::setSourceTexture( ITexture* texture )
{
	SrcTexture = static_cast<CD3D9Texture*>(texture)->getDXTexture();
	TextureSize = texture->getSize();
	ColorFormat = texture->getColorFormat();
	NumMipmaps = texture->getNumMipmaps();
}

void* CD3D9TextureWriter::lock(u32 level, u32& pitch)
{
	D3DLOCKED_RECT rect;
	if (FAILED(SrcTexture->LockRect(level, &rect, NULL_PTR, 0)))
	{
		ASSERT(false);
		return NULL_PTR;
	}

	pitch = rect.Pitch;

	return rect.pBits;
}

void CD3D9TextureWriter::unlock(u32 level)
{
	SrcTexture->UnlockRect(level);
}

bool CD3D9TextureWriter::copyToTexture( ITexture* texture, const recti* descRect )
{
	IDirect3DTexture9* d3dtexture = static_cast<CD3D9Texture*>(texture)->getDXTexture();
	if (SrcTexture == d3dtexture)		//self
		return true;

	if (descRect)
	{
		ASSERT(!texture->hasMipMaps());

		RECT rc;
		rc.left = descRect->UpperLeftCorner.X;
		rc.top = descRect->UpperLeftCorner.Y;
		rc.right = descRect->LowerRightCorner.X;
		rc.bottom = descRect->LowerRightCorner.Y;

		d3dtexture->AddDirtyRect(&rc);
	}

	if(FAILED(Driver->pID3DDevice->UpdateTexture(SrcTexture, d3dtexture)))
	{
		ASSERT(false);
		return false;
	}
	
	return true;
}

void CD3D9TextureWriter::initEmptyData()
{
	u32 pitch;
	void* dest = lock(0, pitch);
	memset(dest, 0, pitch * TextureSize.Height);
	unlock(0);
}

CD3D9TextureWriteServices::CD3D9TextureWriteServices()
{
	SelfTextureWriter = new CD3D9TextureWriter();
}

CD3D9TextureWriteServices::~CD3D9TextureWriteServices()
{
	delete SelfTextureWriter;

	for (T_TextureWriterMap::iterator itr = TextureWriterMap.begin(); itr != TextureWriterMap.end(); ++itr)
	{
		delete itr->second;
	}
	TextureWriterMap.clear();
}

ITextureWriter* CD3D9TextureWriteServices::createTextureWriter( ITexture* texture, bool temp )
{
	if (!g_Engine->getOSInfo()->IsAeroSupport() && texture->getType() == ETT_IMAGE)
	{
		SelfTextureWriter->setSourceTexture(texture);
		return SelfTextureWriter;
	}

	ECOLOR_FORMAT format = texture->getColorFormat();
	dimension2du size = texture->getSize();
	u32 numMipmap = texture->getNumMipmaps();

	T_TextureWriterMap::iterator itr = TextureWriterMap.find(texture);
	if (itr != TextureWriterMap.end())
	{
		return itr->second;
	}

	//not found, create
	CD3D9TextureWriter* writer = new CD3D9TextureWriter(size, format, numMipmap);
	TextureWriterMap[texture] = writer;

	return writer;
}

bool CD3D9TextureWriteServices::removeTextureWriter( ITexture* texture )
{
	T_TextureWriterMap::iterator itr = TextureWriterMap.find(texture);
	if (itr != TextureWriterMap.end())
	{
		delete itr->second;
		TextureWriterMap.erase(itr);
		return true;
	}

	return false;
}

#endif