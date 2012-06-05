#include "stdafx.h"
#include "CD3D9ResourceLoader.h"
#include "mpq_libmpq.h"
#include "mywow.h"
#include "CFileM2.h"
#include "CImageLoaderBLP.h"
#include "CD3D9Texture.h"


volatile bool CD3D9ResourceLoader::StopLoading  = false;

#define  BEGIN_MUTEX(mutex)	\
	if (WAIT_OBJECT_0 != ::WaitForSingleObject(mutex, INFINITE))			\
	{ _ASSERT(false); }

#define END_MUTEX(mutex)  \
	::ReleaseMutex(mutex)

CD3D9ResourceLoader::CD3D9ResourceLoader(bool multithread)
	: MultiThread(multithread)
{
	g_Engine->getDriver()->registerLostReset(this);

	M2Cache_Character.setCacheLimit(5);
	M2Cache_Item.setCacheLimit(10);
	M2Cache_Creature.setCacheLimit(5);
	M2Cache_Particles.setCacheLimit(5);
	M2Cache_Spells.setCacheLimit(5);
	M2Cache_Interface.setCacheLimit(0);
	M2Cache_World.setCacheLimit(5);
	M2Cache_Default.setCacheLimit(0);

	BlpImageCache.setCacheLimit(5);
	BlpTextureCache.setCacheLimit(5);

	M2Loader = new CM2Loader;
	BlpLoader = new CImageLoaderBLP;

	if (MultiThread)
	{
		current.file = NULL;

		m2Mutex = ::CreateMutexA(NULL, FALSE, NULL);
		imageMutex = ::CreateMutexA(NULL, FALSE, NULL);
		textureMutex = ::CreateMutexA(NULL, FALSE, NULL);
		wdtMutex = ::CreateMutexA(NULL, FALSE, NULL);
		adtMutex = ::CreateMutexA(NULL, FALSE, NULL);
		wmoMutex = ::CreateMutexA(NULL, FALSE, NULL);

		//thread
		::InitializeCriticalSection(&cs);
		hLoadingEvent = ::CreateEventA(NULL, FALSE, TRUE, "LoadingEvent"); 
		hLoadedEvent = ::CreateEventA(NULL, FALSE, FALSE, "LoadedEvent");
		ThreadParam = new SParam(this);

		DWORD dwThreadId;
		hThread = ::CreateThread( NULL,              // default security attributes
			0,                 // use default stack size  
			LoadingThreadFunc,          // thread function 
			ThreadParam,             // argument to thread function 
			0,                 // use default creation flags 
			&dwThreadId); 
	}
	
}

CD3D9ResourceLoader::~CD3D9ResourceLoader()
{
	if (MultiThread)
	{
		::CloseHandle(wmoMutex);
		::CloseHandle(adtMutex);
		::CloseHandle(wdtMutex);
		::CloseHandle(textureMutex);
		::CloseHandle(imageMutex);
		::CloseHandle(m2Mutex);

		if (current.file && current.file->getReferenceCount() == 2)		//m2没有被使用
		{
			current.file->drop();
		}

		delete ThreadParam;
		::CloseHandle(hThread);
		::CloseHandle(hLoadedEvent);
		::CloseHandle(hLoadingEvent);
		::DeleteCriticalSection(&cs);	
	}
	
	delete BlpLoader;
	delete M2Loader;

	M2Cache_Default.flushCache();
	M2Cache_Character.flushCache();
	M2Cache_Item.flushCache();
	M2Cache_Creature.flushCache();
	M2Cache_Interface.flushCache();
	M2Cache_Particles.flushCache();
	M2Cache_Spells.flushCache();
	M2Cache_World.flushCache();
	
	BlpTextureCache.flushCache();
	BlpImageCache.flushCache();

}

IFileM2* CD3D9ResourceLoader::loadM2( const c8* filename )
{
	if (MultiThread)
		BEGIN_MUTEX(m2Mutex);

	IFileM2* m2 = NULL;
	IResourceCache<IFileM2>* cache = getM2Cache(filename);

	if (cache)
	{
		m2 = cache->tryLoadFromCache(filename);
		if (m2)
			return m2;
	}
	
	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file && M2Loader->isALoadableFileExtension(filename))
	{	
		m2 = M2Loader->loadM2(file);
		if(m2 && cache)
		{
			m2->setFileName(filename);
			cache->addToCache(filename, m2);
		}
	}

	delete file;

	if (MultiThread)
		END_MUTEX(m2Mutex);

	return m2;
}

IImage* CD3D9ResourceLoader::loadBLP( const c8* filename)
{	
	if (MultiThread)
		BEGIN_MUTEX(imageMutex);

	IImage* image = NULL;
	image = BlpImageCache.tryLoadFromCache(filename);
	if (image)
		return image;

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file && BlpLoader->isALoadableFileExtension(filename))
	{	
		image = BlpLoader->loadImage(file);
		if (image)
		{
			image->setFileName(filename);
			BlpImageCache.addToCache(filename, image);		
		}
	}

	delete file;
	
	if (MultiThread)
		END_MUTEX(imageMutex);

	return image;
}

ITexture* CD3D9ResourceLoader::loadTexture( const c8* filename, bool mipmap )
{
	if (MultiThread)
		BEGIN_MUTEX(textureMutex);

	ITexture* tex = NULL;
	tex = BlpTextureCache.tryLoadFromCache(filename);
	if (tex)
		return tex;

	IImage* image = loadBLP(filename);
	if (image)
	{
		CD3D9Texture* texd3d9 = new CD3D9Texture(image, mipmap);
		image->drop();

		tex = texd3d9;
		tex->setFileName(filename);
		BlpTextureCache.addToCache(filename, tex);
	}

	if (MultiThread)
		END_MUTEX(textureMutex);

	return tex;
}

DWORD WINAPI CD3D9ResourceLoader::LoadingThreadFunc( LPVOID lpParam )
{
	SParam* param = (SParam*)lpParam;
	CD3D9ResourceLoader* loader = param->loader;

	STask task;
	while (!StopLoading)
	{
		if (!loader->taskList.empty())
		{
			::WaitForSingleObject(loader->hLoadingEvent, INFINITE);
			::ResetEvent(loader->hLoadedEvent);

			::EnterCriticalSection(&loader->cs);
			task = loader->taskList.front();
			::LeaveCriticalSection(&loader->cs);

			task.file = loader->loadM2(task.filename);

			::EnterCriticalSection(&loader->cs);
			loader->current = task;
			loader->taskList.pop_front();
			::LeaveCriticalSection(&loader->cs);

			::SetEvent(loader->hLoadedEvent);
		}
		else
		{
			::Sleep(2);
		}
	}

	return 0;
}

void CD3D9ResourceLoader::beginLoadM2( c8* filename )
{
	if (!MultiThread || !hasFileExtensionA(filename, "m2"))
		return;

	STask task = {0};
	strcpy_s(task.filename, MAX_PATH, filename);

	::EnterCriticalSection(&cs);

	taskList.push_back(task);

	::LeaveCriticalSection(&cs);
}

bool CD3D9ResourceLoader::cancelLoadM2(c8* filename)
{
	if (!MultiThread || !hasFileExtensionA(filename, "m2"))
		return false;

	::EnterCriticalSection(&cs);

	for (T_TaskList::iterator itr = taskList.begin(); itr != taskList.end(); ++itr)
	{
		if (_stricmp((*itr).filename, filename) ==0)
		{
			taskList.erase(itr);
			::LeaveCriticalSection(&cs);
			return true;
		}
	}

	::LeaveCriticalSection(&cs);
	return false;
}

bool CD3D9ResourceLoader::m2LoadCompleted()
{
	if (!MultiThread)
		return false;
	return WAIT_OBJECT_0 == ::WaitForSingleObject(hLoadedEvent, NULL);
}

void CD3D9ResourceLoader::stopLoadingM2()
{
	if(MultiThread)
		::ResetEvent(hLoadingEvent); 
}

void CD3D9ResourceLoader::resumeLoadingM2()
{ 
	if(MultiThread)
		::SetEvent(hLoadingEvent); 
}

IResourceCache<IFileM2>* CD3D9ResourceLoader::getM2Cache( const c8* filename )
{
	M2Type type = getM2Type(filename);
	switch(type)
	{
	case MT_CHARACTER:
		return &M2Cache_Character;
	case MT_ITEM:
		return &M2Cache_Item;
	case MT_CREATRUE:
		return &M2Cache_Creature;
	case MT_PARTCILES:
		return &M2Cache_Particles;
	case MT_SPELLS:
		return &M2Cache_Spells;
	case MT_INTERFACE:
		return &M2Cache_Interface;
	case MT_WORLD:
		return &M2Cache_World;
	default:
		return &M2Cache_Default;
	}
}

void CD3D9ResourceLoader::onLost()
{
	stopLoadingM2();
}

void CD3D9ResourceLoader::onReset()
{
	resumeLoadingM2();
}

void CD3D9ResourceLoader::endLoadingM2()
{
	if (MultiThread)
	{
		//thread
		CD3D9ResourceLoader::StopLoading = true;
		if( WAIT_OBJECT_0 != ::WaitForSingleObject(hThread, INFINITE))
		{
			_ASSERT(false);
		}
	}
}