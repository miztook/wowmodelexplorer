#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "ITextureWriteServices.h"
#include "core.h"
#include <unordered_map>

class CD3D9Driver;

class CD3D9TextureWriter : public ITextureWriter
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9TextureWriter);

	CD3D9TextureWriter(const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap);
	CD3D9TextureWriter();
	~CD3D9TextureWriter();

	void setSourceTexture(ITexture* texture);

	friend class CD3D9TextureWriteServices;

public:
	virtual void* lock(u32 level, u32& pitch);
	virtual void unlock(u32 level);
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = NULL_PTR);
	virtual void initEmptyData();

private:
	IDirect3DTexture9*	 SrcTexture;
	CD3D9Driver*	Driver;
	bool NeedCreate;
};


class CD3D9TextureWriteServices : public ITextureWriteServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9TextureWriteServices);

public:
	CD3D9TextureWriteServices();
	~CD3D9TextureWriteServices();
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
	CD3D9TextureWriter*		SelfTextureWriter;
};

#endif