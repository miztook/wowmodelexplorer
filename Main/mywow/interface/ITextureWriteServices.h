#pragma once

#include "core.h"

class ITexture;

//在d3d9中，纹理都是managed类型(lockable)，textureWriter是自身的一个代理
//在d3d9ex, d3d10, d3d11中，纹理必须是显存纹理(unlockable)，textureWriter就是专门用内存纹理写显存纹理的工具
class ITextureWriter
{
public:
	explicit ITextureWriter(uint32_t numMipmap) : NumMipmaps(numMipmap), ColorFormat(ECF_UNKNOWN) {}
	virtual ~ITextureWriter() {}

public:
	virtual void* lock(uint32_t level, uint32_t& pitch) = 0;
	virtual void unlock(uint32_t level) = 0;
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = nullptr) = 0;
	virtual void initEmptyData() = 0;

public:
	dimension2du	TextureSize;
	ECOLOR_FORMAT	ColorFormat;
	uint32_t	NumMipmaps;
};

class ITextureWriteServices
{
public:
	virtual ~ITextureWriteServices() {}

public:
	virtual ITextureWriter* createTextureWriter(ITexture* texture, bool temp)= 0;
	virtual bool removeTextureWriter(ITexture* texture) = 0;
};