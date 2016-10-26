#pragma once

#include "IResourceLoader.h"
#include "IResourceCache.h"

#include "CSysThread.h"

#include "CFileM2Loader.h"
#include "CFileWMOLoader.h"
#include "CFileADTLoader.h"
#include "CFileWDTLoader.h"
#include "CImageLoaderBLP.h"
#include "CImageLoaderPVR.h"
#include "CImageLoaderJPG.h"
#include "CImageLoaderPNG.h"
#include "CImageLoaderKTX.h"

class CResourceLoader : public IResourceLoader
{
public:
	CResourceLoader();
	virtual ~CResourceLoader();

public:
	virtual IImage* loadJPGAsImage(const c8* filename);
	virtual IImage* loadPNGAsImage(const c8* filename);
	virtual IImage* loadBLPAsImage(const c8* filename, bool changeRB = false);
	virtual IBLPImage* loadBLP(const c8* filename);
	virtual IImage* loadPVRAsImage(const c8* filename);
	virtual IPVRImage* loadPVR(const c8* filename);
	virtual IImage* loadKTXAsImage(const c8* filename);
	virtual IKTXImage* loadKTX(const c8* filename);

	virtual IFileM2* loadM2(const c8* filename, bool videobuild = true);
	virtual IFileWDT* loadWDT(const c8* filename, s32 mapid, bool simple = false);
	virtual IFileADT* loadADT(const c8* filename, bool simple, bool videobuild = true);
	virtual IFileADT* loadADTTextures(const c8* filename);
	virtual IFileWMO* loadWMO(const c8* filename, bool videobuild = true);

	virtual void registerM2Loaded(IM2LoadCallback* callback);
	virtual void removeM2Loaded(IM2LoadCallback* callback);

	virtual void clearCurrentTask();

	virtual void setCacheLimit(E_CACHE_TYPE type, u32 limit);
	virtual u32 getCacheLimit(E_CACHE_TYPE type) const;

	//m2 async loading
	virtual void beginLoadM2(const c8* filename, const SParamBlock& param);
	virtual bool m2LoadCompleted();

	//wmo async loading
	virtual void beginLoadWMO(const c8* filename, const SParamBlock& param);
	virtual bool wmoLoadCompleted();

	//adt async loading
	virtual void beginLoadADT(const c8* filename, const SParamBlock& param);
	virtual bool adtLoadCompleted();

	virtual void beginLoading();
	virtual void cancelAll(E_TASK_TYPE type);
	virtual void waitLoadingSuspend(); 
	virtual void resumeLoading();
	virtual void endLoading();

protected:	
	struct SParam
	{
		CResourceLoader* loader;
	} ;

	IResourceCache<IFileM2>* getM2Cache(const c8* filename);

	void clearLoadedFiles();

	static int LoadingThreadFunc( void* lpParam ); 

protected:
	SParam		ThreadParam;

	thread_type		hThread;

	lock_type		cs;

	lock_type		m2CS;
	lock_type		imageCS;
	lock_type		blpCS;
	lock_type		pvrCS;
	lock_type		ktxCS;
	lock_type		textureCS;
	lock_type		wdtCS;
	lock_type		adtCS;
	lock_type		wmoCS;

	event_type		hLoadingEvent;

	typedef std::list<STask, qzone_allocator<STask> >		T_TaskList;
	T_TaskList taskList;	

	typedef std::list<IM2LoadCallback*, qzone_allocator<IM2LoadCallback*> > T_M2LoadedList;
	T_M2LoadedList	M2LoadedCallbackList;

protected:
	IResourceCache<IFileM2>			M2Cache_Character;
	IResourceCache<IFileM2>			M2Cache_Item;
	IResourceCache<IFileM2>			M2Cache_Creature;
	IResourceCache<IFileM2>			M2Cache_Particles;
	IResourceCache<IFileM2>			M2Cache_Spells;
	IResourceCache<IFileM2>			M2Cache_Interface;
	IResourceCache<IFileM2>			M2Cache_World;
	IResourceCache<IFileM2>			M2Cache_Default;			//д╛хо

	IResourceCache<IImage>			ImageCache;
	IResourceCache<IBLPImage>		BlpImageCache;		
	IResourceCache<IPVRImage>		PvrImageCache;
	IResourceCache<IKTXImage>		KtxImageCache;
	IResourceCache<ITexture>			TextureCache;

	IResourceCache<IFileADT>			ADTCache;
	IResourceCache<IFileWMO>		WMOCache;

	CImageLoaderJPG		JpgLoader;
	CImageLoaderPNG		PngLoader;
	CImageLoaderBLP		BlpLoader;
	CImageLoaderPVR		PvrLoader;
	CImageLoaderKTX		KtxLoader;
	CM2Loader			M2Loader;
	CWDTLoader		WDTLoader;
	CADTLoader		ADTLoader;
	CWMOLoader		WMOLoader;

	volatile bool StopLoading;
	volatile bool Suspended;
	bool	MultiThread;
};