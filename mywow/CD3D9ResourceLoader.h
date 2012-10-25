#pragma once

#include "IResourceLoader.h"
#include "IResourceCache.h"

class CM2Loader;
class CImageLoaderBLP;
class CWDTLoader;
class CADTLoader;
class CWMOLoader;
class CD3D9TexLoader;

class CD3D9ResourceLoader : public IResourceLoader
{
public:
	CD3D9ResourceLoader(bool multithread);
	~CD3D9ResourceLoader();

public:
	virtual IFileM2* loadM2(const c8* filename);
	virtual IImage* loadBLPAsImage(const c8* filename);
	virtual IBLPImage* loadBLP(const c8* filename);
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true);
	virtual IFileWDT* loadWDT(const c8* filename, s32 mapid);
	virtual IFileADT* loadADT(const c8* filename);
	virtual IFileWMO* loadWMO(const c8* filename);

	virtual const STask& getCurrentTask() const { return currentTask; }
	virtual void clearCurrentTask();

	//m2 async loading
	virtual void beginLoadM2(const c8* filename, const SParamBlock& param);
	virtual bool cancelLoadM2(const c8* filename);
	virtual bool m2LoadCompleted() const;

	//wmo async loading
	virtual void beginLoadWMO(const c8* filename, const SParamBlock& param);
	virtual bool cancelLoadWMO(const c8* filename);
	virtual bool wmoLoadCompleted() const;

	virtual void beginLoadADT(const c8* filename, const SParamBlock& param);
	virtual bool cancelLoadADT(const c8* filename);
	virtual bool adtLoadCompleted() const;

	virtual void cancelAll(E_TASK_TYPE type);
	virtual void stopLoading(); 
	virtual void resumeLoading();
	virtual void endLoading();

private:
	IResourceCache<IFileM2>* getM2Cache(const c8* filename);

	bool	MultiThread;
private:
	struct SParam
	{
		SParam(CD3D9ResourceLoader* p) : loader(p) {}

		CD3D9ResourceLoader* loader;
	} ;

	SParam*		ThreadParam;
	HANDLE		hThread;

	static DWORD WINAPI LoadingThreadFunc( LPVOID lpParam ); 
	volatile static bool StopLoading;

	HANDLE		hLoadingEvent;
	HANDLE		hLoadedEvent;
	HANDLE		hLoadingStoppedEvent;

	typedef std::list<STask, qzone_allocator<STask>>		T_TaskList;
	T_TaskList taskList;	

	STask currentTask;

	CRITICAL_SECTION	cs;

	HANDLE		m2Mutex;
	HANDLE		imageMutex;
	HANDLE		blpMutex;
	HANDLE		textureMutex;
	HANDLE		wdtMutex;
	HANDLE		adtMutex;
	HANDLE		wmoMutex;
	
private:
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
	IResourceCache<ITexture>			BlpTextureCache;

	IResourceCache<IFileADT>			ADTCache;
	IResourceCache<IFileWMO>		WMOCache;

	CM2Loader*			M2Loader;
	CImageLoaderBLP*		BlpLoader;
	CWDTLoader*		WDTLoader;
	CADTLoader*		ADTLoader;
	CWMOLoader*		WMOLoader;
	CD3D9TexLoader*		TexLoader;
};