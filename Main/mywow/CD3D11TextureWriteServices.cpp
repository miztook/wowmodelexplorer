#include "stdafx.h"
#include "CD3D11TextureWriteServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"
#include "CD3D11Helper.h"
#include "CD3D11Texture.h"

CD3D11TextureWriter::CD3D11TextureWriter( const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap )
	: ITextureWriter(numMipmap)
{
	Driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());

	TextureSize = size;
	ColorFormat = format;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	desc.ArraySize = 1;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.Format = CD3D11Helper::getDXGIFormatFromColorFormat(format);;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = TextureSize.Width;
	desc.Height = TextureSize.Height;
	desc.BindFlags = 0;
	desc.MipLevels = NumMipmaps;
	
	HRESULT hr = Driver->pID3DDevice11->CreateTexture2D( &desc, NULL_PTR, &SrcTexture );
	if (FAILED(hr))
	{
		ASSERT(false);
	}
}

CD3D11TextureWriter::~CD3D11TextureWriter()
{
	SAFE_RELEASE_STRICT(SrcTexture);
}

void* CD3D11TextureWriter::lock( u32 level, u32& pitch )
{
	D3D11_MAPPED_SUBRESOURCE mappedData;

	void* pData = NULL_PTR;
	//ImmediateContext
	HRESULT hr = Driver->ImmediateContext->Map( SrcTexture, level, D3D11_MAP_WRITE, 0, &mappedData );
	if (FAILED(hr))
	{
		ASSERT(false);
		return NULL_PTR;
	}

	pitch = mappedData.RowPitch;
	pData = mappedData.pData;
	
	return pData;
}

void CD3D11TextureWriter::unlock( u32 level )
{
	//ImmediateContext
	Driver->ImmediateContext->Unmap(SrcTexture, level);
}

bool CD3D11TextureWriter::copyToTexture( ITexture* texture, const recti* descRect )
{
	ID3D11Texture2D* d3dtexture = static_cast<CD3D11Texture*>(texture)->getDXTexture();

	if (descRect)
	{
		ASSERT(!texture->hasMipMaps());

		D3D11_BOX box;
		box.left = descRect->UpperLeftCorner.X;
		box.right = descRect->LowerRightCorner.X;
		box.top = descRect->UpperLeftCorner.Y;
		box.bottom = descRect->LowerRightCorner.Y;
		box.front = 0;
		box.back = 1;

		//ImmediateContext
		Driver->ImmediateContext->CopySubresourceRegion(d3dtexture, 0, box.left, box.top, 0, SrcTexture, 0, &box);
	}
	else
	{
		//ImmediateContext
		Driver->ImmediateContext->CopyResource(d3dtexture, SrcTexture);
	}
	return true;
}

void CD3D11TextureWriter::initEmptyData()
{
	u32 pitch;
	void* dest = lock(0, pitch);
	memset(dest, 0, pitch * TextureSize.Height);
	unlock(0);
}

CD3D11TextureWriteServices::CD3D11TextureWriteServices()
{

}

CD3D11TextureWriteServices::~CD3D11TextureWriteServices()
{
	for (T_TextureWriterMap::iterator itr = TextureWriterMap.begin(); itr != TextureWriterMap.end(); ++itr)
	{
		delete itr->second;
	}
	TextureWriterMap.clear();
}

ITextureWriter* CD3D11TextureWriteServices::createTextureWriter( ITexture* texture, bool temp )
{
	ECOLOR_FORMAT format = texture->getColorFormat();
	dimension2du size = texture->getSize();
	u32 numMipmap = texture->getNumMipmaps();

	T_TextureWriterMap::iterator itr = TextureWriterMap.find(texture);
	if (itr != TextureWriterMap.end())
	{
		return itr->second;
	}

	//not found, create
	CD3D11TextureWriter* writer = new CD3D11TextureWriter(size, format, numMipmap);
	TextureWriterMap[texture] = writer;

	return writer;
}

bool CD3D11TextureWriteServices::removeTextureWriter( ITexture* texture )
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