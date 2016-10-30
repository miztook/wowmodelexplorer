#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "ITextureWriteServices.h"
#include "core.h"
#include <unordered_map>
#include <map>

class COpenGLDriver;

class COpenGLTextureWriter : public ITextureWriter
{
private:
	COpenGLTextureWriter(const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap, bool bTempMemory);
	~COpenGLTextureWriter();

	friend class COpenGLTextureWriteServices;

public:
	virtual void* lock(u32 level, u32& pitch);
	virtual void unlock(u32 level);
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = NULL_PTR);
	virtual void initEmptyData();

private:
	struct SMipData
	{
		u32 pitch;
		u8* data;
	};

private:
	SMipData*		MipData;
	bool		TempMemory;
};


class COpenGLTextureWriteServices : public ITextureWriteServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLTextureWriteServices);

public:
	COpenGLTextureWriteServices();
	~COpenGLTextureWriteServices();
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