#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "ITextureWriteServices.h"
#include "core.h"
#include <list>
#include <unordered_map>

class CGLES2Driver;

class CGLES2TextureWriter : public ITextureWriter
{
private:
	CGLES2TextureWriter(const dimension2du& size, ECOLOR_FORMAT format, u32 numMipmap, bool bTempMemory);
	~CGLES2TextureWriter();

	friend class CGLES2TextureWriteServices;

public:
	virtual void* lock(u32 level, u32& pitch);
	virtual void unlock(u32 level);
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = NULL);
	virtual void initEmptyData();

private:
	struct SMipData
	{
		u32 pitch;
		u8* data;
	};

private:
	CGLES2Driver*	Driver;
	SMipData*		MipData;
	bool		TempMemory;
};


class CGLES2TextureWriteServices : public ITextureWriteServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2TextureWriteServices);

public:
	CGLES2TextureWriteServices();
	~CGLES2TextureWriteServices();
public:
	virtual ITextureWriter* createTextureWriter(ITexture* texture, bool temp);
	virtual bool removeTextureWriter(ITexture* texture);

private:
	typedef std::unordered_map<ITexture*, ITextureWriter*>	T_TextureWriterMap;
	T_TextureWriterMap		TextureWriterMap;
};

#endif