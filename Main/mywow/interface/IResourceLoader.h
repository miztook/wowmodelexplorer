#pragma once

#include "base.h"

template <class T>
class IReferenceCounted;

class IImage;
class IBLPImage;
class IPVRImage;
class IKTXImage;
class ITexture;
class IFileM2;
class IFileWDT;
class IFileADT;
class IFileWMO;

class IM2LoadCallback;

class IResourceLoader
{
public:
	virtual ~IResourceLoader() {}

public:
	enum E_CACHE_TYPE
	{
		ECT_M2_CHARACTER = 0,
		ECT_M2_ITEM,
		ECT_M2_CREATURE,
		ECT_M2_PARTICLES,
		ECT_M2_SPELLS,
		ECT_M2_INTERFACE,
		ECT_M2_WORLD,
		ECT_M2_DEFAULT,

		ECT_IMAGE,
		ECT_BLP_IMAGE,
		ECT_PVR_IMAGE,
		ECT_KTX_IMAGE,
		ECT_TEXTURE,

		ECT_ADT,
		ECT_WMO,
	};

	enum E_TASK_TYPE
	{
		ET_NONE = 0,
		ET_M2,
		ET_WMO,
		ET_ADT,
	};

	struct SParamBlock
	{
		void* param1;		//sender
		void* param2;
		void* param3;
	};

	struct STask
	{
		c8 filename[QMAX_PATH];
		SParamBlock		param;
		void* file;
		E_TASK_TYPE	type;
		bool	cancel;		//标记为cancel，处理时跳过
	};

public:
	virtual IImage* loadJPGAsImage(const c8* filename) = 0;
	virtual IImage* loadPNGAsImage(const c8* filename) = 0;
	virtual IImage* loadBLPAsImage(const c8* filename, bool changeRB = false) = 0;
	virtual IBLPImage* loadBLP(const c8* filename) = 0;
	virtual IImage* loadPVRAsImage(const c8* filename) = 0;
	virtual IPVRImage* loadPVR(const c8* filename) = 0;	
	virtual IImage* loadKTXAsImage(const c8* filename) = 0;
	virtual IKTXImage* loadKTX(const c8* filename) = 0;

	virtual IFileM2* loadM2(const c8* filename, bool videobuild = true) = 0;
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true) = 0;
	virtual IFileWDT* loadWDT(const c8* filename, s32 mapid, bool simple = false) = 0;
	virtual IFileADT* loadADT(const c8* filename, bool simple, bool videobuild = true) = 0;
	virtual IFileADT* loadADTTextures(const c8* filename) = 0;
	virtual IFileWMO* loadWMO(const c8* filename, bool videobuild = true) = 0;

	virtual void registerM2Loaded(IM2LoadCallback* callback) = 0;
	virtual void removeM2Loaded(IM2LoadCallback* callback) = 0;

	const STask& getCurrentTask() const { return currentTask; }
	virtual void clearCurrentTask() = 0;

	virtual void setCacheLimit(E_CACHE_TYPE type, u32 limit) = 0;
	virtual u32 getCacheLimit(E_CACHE_TYPE type) const= 0;

	//m2 async loading
	virtual void beginLoadM2(const c8* filename, const SParamBlock& param) = 0;
	virtual bool m2LoadCompleted() = 0;

	//wmo async loading
	virtual void beginLoadWMO(const c8* filename, const SParamBlock& param) = 0;
	virtual bool wmoLoadCompleted() = 0;

	//adt async loading
	virtual void beginLoadADT(const c8* filename, const SParamBlock& param) = 0;
	virtual bool adtLoadCompleted() = 0;

	virtual void beginLoading() = 0;
	virtual void cancelAll(E_TASK_TYPE type) = 0;
	virtual void waitLoadingSuspend() = 0;
	virtual void resumeLoading() = 0;
	virtual void endLoading()  = 0;

protected:
	STask currentTask;
};