#pragma once

#include "base.h"

template <class T>
class IReferenceCounted;

class IFileM2;
class IImage;
class ITexture;

class IResourceLoader
{
public:
	virtual ~IResourceLoader() { }

public:
	struct STask
	{
		c8 filename[MAX_PATH];
		IReferenceCounted<IFileM2>* file;
	};

public:
	virtual IFileM2* loadM2(const c8* filename) = 0;
	virtual IImage* loadBLP(const c8* filename) = 0;
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true) = 0;

	//m2 async loading
	virtual void beginLoadM2(c8* filename) = 0;
	virtual bool cancelLoadM2(c8* filename) = 0;
	virtual STask getCurrentM2Task() const = 0; 
	virtual bool m2LoadCompleted() = 0;
	virtual void stopLoadingM2() = 0;
	virtual void resumeLoadingM2() = 0;
	virtual void endLoadingM2()  = 0;
};