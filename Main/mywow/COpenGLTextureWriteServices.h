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
	COpenGLTextureWriter(const dimension2du& size, ECOLOR_FORMAT format, uint32_t numMipmap, bool bTempMemory);
	~COpenGLTextureWriter();

	friend class COpenGLTextureWriteServices;

public:
	virtual void* lock(uint32_t level, uint32_t& pitch);
	virtual void unlock(uint32_t level);
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = nullptr);
	virtual void initEmptyData();

private:
	struct SMipData
	{
		uint32_t pitch;
		uint8_t* data;
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