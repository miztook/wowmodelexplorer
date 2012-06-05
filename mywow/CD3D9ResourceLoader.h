#pragma once

#include "IResourceLoader.h"
#include "IResourceCache.h"

class CM2Loader;
class CImageLoaderBLP;

class CD3D9ResourceLoader : public IResourceLoader, public ILostResetCallback
{
public:
	CD3D9ResourceLoader(bool multithread);
	~CD3D9ResourceLoader();

public:
	virtual IFileM2* loadM2(const c8* filename);
	virtual IImage* loadBLP(const c8* filename);
	virtual ITexture* loadTexture(const c8* filename, bool mipmap = true);

	virtual void beginLoadM2(c8* filename);
	virtual bool cancelLoadM2(c8* filename);
	virtual STask getCurrentM2Task() const { return current; }
	virtual bool m2LoadCompleted();
	virtual void stopLoadingM2();
	virtual void resumeLoadingM2();
	virtual void endLoadingM2();

public:
	virtual void onLost();
	virtual void onReset();

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

	CRITICAL_SECTION	cs;
	HANDLE		hLoadingEvent;
	HANDLE		hLoadedEvent;

	typedef std::list<STask, qzone_allocator<STask>>		T_TaskList;
	T_TaskList taskList;	

	STask current;

	HANDLE		m2Mutex;
	HANDLE		imageMutex;
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

	IResourceCache<IImage>			BlpImageCache;
	IResourceCache<ITexture>		BlpTextureCache;

	CM2Loader*			M2Loader;
	CImageLoaderBLP*		BlpLoader;
};