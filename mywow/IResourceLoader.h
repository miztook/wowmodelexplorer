#pragma once

#include "base.h"

template <class T>
class IReferenceCounted;

class IFileM2;
class IImage;
class IBLPImage;
class ITexture;
class IFileWDT;
class IFileADT;
class IFileWMO;

class IResourceLoader
{
public:
	virtual ~IResourceLoader() {}

public:
	enum E_TASK_TYPE
	{
		ET_M2 = 0,
		ET_WMO,
		ET_ADT,
	};

	struct SParamBlock
	{
		void* param1;
		void* param2;
		void* param3;
		void* param4;
	};

	struct STask
	{
		c8 filename[MAX_PATH];
		void* file;
		E_TASK_TYPE	type;
		SParamBlock		param;
	};

public:
	virtual IFileM2* loadM2(const c8* filename) = 0;
	virtual IImage* loadBLPAsImage(const c8* filename) = 0;
	virtual IBLPImage* loadBLP(const c8* filename) = 0;
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true) = 0;
	virtual IFileWDT* loadWDT(const c8* filename, s32 mapid) = 0;
	virtual IFileADT* loadADT(const c8* filename) = 0;
	virtual IFileWMO* loadWMO(const c8* filename) = 0;

	virtual const STask& getCurrentTask() const = 0;
	virtual void clearCurrentTask() = 0;

	//m2 async loading
	virtual void beginLoadM2(const c8* filename, const SParamBlock& param) = 0;
	virtual bool cancelLoadM2(const c8* filename) = 0;
	virtual bool m2LoadCompleted() const = 0;

	//wmo async loading
	virtual void beginLoadWMO(const c8* filename, const SParamBlock& param) = 0;
	virtual bool cancelLoadWMO(const c8* filename) = 0;
	virtual bool wmoLoadCompleted() const = 0;

	//adt async loading
	virtual void beginLoadADT(const c8* filename, const SParamBlock& param) = 0;
	virtual bool cancelLoadADT(const c8* filename) = 0;
	virtual bool adtLoadCompleted() const = 0;

	virtual void cancelAll(E_TASK_TYPE type) = 0;
	virtual void stopLoading() = 0;
	virtual void resumeLoading() = 0;
	virtual void endLoading()  = 0;

};