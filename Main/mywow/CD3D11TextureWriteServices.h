#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "ITextureWriteServices.h"
#include "core.h"
#include <unordered_map>
#include <map>

class CD3D11Driver;

class CD3D11TextureWriter : public ITextureWriter
{
private:
	CD3D11TextureWriter(const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap);
	~CD3D11TextureWriter();

	friend class CD3D11TextureWriteServices;

public:
	virtual void* lock(u32 level, u32& pitch);
	virtual void unlock(u32 level);
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = NULL_PTR);
	virtual void initEmptyData();

private:
	CD3D11Driver*		Driver;
	ID3D11Texture2D*	SrcTexture;
};

class CD3D11TextureWriteServices : public ITextureWriteServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11TextureWriteServices);

public:
	CD3D11TextureWriteServices();
	~CD3D11TextureWriteServices();
public:
	virtual ITextureWriter* createTextureWriter(ITexture* texture, bool temp);
	virtual bool removeTextureWriter(ITexture* texture);

private:
#ifdef USE_QALLOCATOR
	typedef std::map<ITexture*, ITextureWriter*, std::less<ITexture*>, qzone_allocator<std::pair<ITexture*, ITextureWriter*>>>	T_TextureWriterMap;
#else
	typedef std::unordered_map<ITexture*, ITextureWriter*>	T_TextureWriterMap;
#endif

	T_TextureWriterMap		TextureWriterMap;
};

#endif