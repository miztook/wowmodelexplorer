#include "stdafx.h"
#include "CD3D9ResourceLoader.h"
#include "mpq_libmpq.h"
#include "mywow.h"
#include "CFileM2.h"
#include "CImageLoaderBLP.h"
#include "CBLPImage.h"
#include "CD3D9Texture.h"
#include "CFileWDT.h"
#include "CFileADT.h"
#include "CFileWMO.h"


volatile bool CD3D9ResourceLoader::StopLoading  = false;

#define  BEGIN_MUTEX(mutex)	\
	if (WAIT_OBJECT_0 != ::WaitForSingleObject(mutex, INFINITE))			\
	{ _ASSERT(false); }

#define END_MUTEX(mutex)  \
	::ReleaseMutex(mutex)

CD3D9ResourceLoader::CD3D9ResourceLoader(bool multithread)
	: MultiThread(multithread)
{
	M2Cache_Character.setCacheLimit(5);
	M2Cache_Item.setCacheLimit(10);
	M2Cache_Creature.setCacheLimit(5);
	M2Cache_Particles.setCacheLimit(5);
	M2Cache_Spells.setCacheLimit(5);
	M2Cache_Interface.setCacheLimit(0);
	M2Cache_World.setCacheLimit(5);
	M2Cache_Default.setCacheLimit(0);

	ImageCache.setCacheLimit(5);
	BlpImageCache.setCacheLimit(0);
	BlpTextureCache.setCacheLimit(0);

	ADTCache.setCacheLimit(5);
	WMOCache.setCacheLimit(2);

	M2Loader = new CM2Loader;
	BlpLoader = new CImageLoaderBLP;
	WDTLoader = new CWDTLoader;
	ADTLoader = new CADTLoader;
	WMOLoader = new CWMOLoader;
	TexLoader = new CD3D9TexLoader;

	if (MultiThread)
	{
		currentTask.file = NULL;

		::InitializeCriticalSection(&cs);
		m2Mutex = ::CreateMutexA(NULL, FALSE, NULL);
		imageMutex = ::CreateMutexA(NULL, FALSE, NULL);
		blpMutex = ::CreateMutexA(NULL, FALSE, NULL);
		textureMutex = ::CreateMutexA(NULL, FALSE, NULL);
		wdtMutex = ::CreateMutexA(NULL, FALSE, NULL);
		adtMutex = ::CreateMutexA(NULL, FALSE, NULL);
		wmoMutex = ::CreateMutexA(NULL, FALSE, NULL);

		//thread
		hLoadingEvent = ::CreateEventA(NULL, FALSE, TRUE, "LoadingEvent"); 
		hLoadedEvent = ::CreateEventA(NULL, FALSE, FALSE, "LoadedEvent");
		hLoadingStoppedEvent = ::CreateEventA(NULL, TRUE, TRUE, "StopLoadingEvent");
		ThreadParam = new SParam(this);

		DWORD dwThreadId;
		hThread = ::CreateThread( NULL,              // default security attributes
			0,                 // use default stack size  
			LoadingThreadFunc,          // thread function 
			ThreadParam,             // argument to thread function 
			0,                 // use default creation flags 
			&dwThreadId); 

		//::SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
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
		::CloseHandle(blpMutex);
		::CloseHandle(imageMutex);
		::CloseHandle(m2Mutex);
		::DeleteCriticalSection(&cs); 

		if( WAIT_OBJECT_0 != ::WaitForSingleObject(hThread, 0))
		{
			_ASSERT(false);
		}

		if (currentTask.file)
		{
			switch(currentTask.type)
			{
			case ET_M2:
				{
					IReferenceCounted<IFileM2>* iref = (IReferenceCounted<IFileM2>*)currentTask.file;
					if(iref->getReferenceCount() == 2)
						iref->drop();
				}
				break;
			case ET_WMO:
				{
					IReferenceCounted<IFileWMO>* iref = (IReferenceCounted<IFileWMO>*)currentTask.file;
					if(iref->getReferenceCount() == 2)
						iref->drop();
				}
				break;
			case ET_ADT:
				{
					IReferenceCounted<IFileADT>* iref = (IReferenceCounted<IFileADT>*)currentTask.file;
					if(iref->getReferenceCount() == 2)
						iref->drop();
				}
				break;
			default:
				_ASSERT(false);
				break;
			}		
		}
		
		delete ThreadParam;
		::CloseHandle(hThread);
		::CloseHandle(hLoadingStoppedEvent);
		::CloseHandle(hLoadedEvent);
		::CloseHandle(hLoadingEvent);
	}
	
	delete TexLoader;
	delete WMOLoader;
	delete ADTLoader;
	delete WDTLoader;
	delete BlpLoader;
	delete M2Loader;

	ADTCache.flushCache();
	WMOCache.flushCache();

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
	ImageCache.flushCache();

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
		{
			if (MultiThread)
				END_MUTEX(m2Mutex);

			return m2;
		}
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

IImage* CD3D9ResourceLoader::loadBLPAsImage( const c8* filename)
{	
	if (MultiThread)
		BEGIN_MUTEX(imageMutex);

	IImage* image = NULL;
	image = ImageCache.tryLoadFromCache(filename);
	if (image)
	{
		if (MultiThread)
			END_MUTEX(imageMutex);
		return image;
	}

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file && BlpLoader->isALoadableFileExtension(filename))
	{	
		image = BlpLoader->loadImage(file);
		if (image)
		{
			image->setFileName(filename);
			ImageCache.addToCache(filename, image);		
		}
	}

	delete file;
	
	if (MultiThread)
		END_MUTEX(imageMutex);

	return image;
}

IBLPImage* CD3D9ResourceLoader::loadBLP( const c8* filename )
{
	if (MultiThread)
		BEGIN_MUTEX(blpMutex);

	IBLPImage* image = NULL;
	image = BlpImageCache.tryLoadFromCache(filename);
	if (image)
	{
		if (MultiThread)
			END_MUTEX(blpMutex);
		return image;
	}

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file && BlpLoader->isALoadableFileExtension(filename))
	{	
		image = new CBLPImage();
		if (image->loadFile(file))
		{
			image->setFileName(filename);
			BlpImageCache.addToCache(filename, image);		
		}
		else
		{
			delete image;
			image = NULL;
		}
	}

	delete file;

	if (MultiThread)
		END_MUTEX(blpMutex);

	return image;
}

ITexture* CD3D9ResourceLoader::loadTexture( const c8* filename, bool mipmap )
{
	if (MultiThread)
		BEGIN_MUTEX(textureMutex);

	ITexture* tex = NULL;
	tex = BlpTextureCache.tryLoadFromCache(filename);
	if (tex)
	{
		if (MultiThread)
			END_MUTEX(textureMutex);

		return tex;
	}

// 	IImage* image = loadBLPAsImage(filename);
// 	if (image)
// 	{
// 		tex = TexLoader->loadTexture(image, mipmap);
// 		image->drop();
// 
// 		tex->setFileName(filename);
// 		BlpTextureCache.addToCache(filename, tex);
// 	}
	IBLPImage* blp = loadBLP(filename);
	if (blp)
	{
		tex = TexLoader->loadTexture(blp, mipmap);
		blp->drop();

		_ASSERT(tex);
		tex->setFileName(filename);
		BlpTextureCache.addToCache(filename, tex);
	}

	if (MultiThread)
		END_MUTEX(textureMutex);

	return tex;
}


IFileWDT* CD3D9ResourceLoader::loadWDT( const c8* filename, s32 mapid )
{
	if (MultiThread)
		BEGIN_MUTEX(wdtMutex);

	IFileWDT* wdt = NULL;

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file && WDTLoader->isALoadableFileExtension(filename))	
		wdt = WDTLoader->loadWDT(file, mapid);

	delete file;

	if (MultiThread)
		END_MUTEX(wdtMutex);

	return wdt;
}

IFileADT* CD3D9ResourceLoader::loadADT( const c8* filename )
{
	if (MultiThread)
		BEGIN_MUTEX(adtMutex);

	IFileADT* adt = NULL;
	adt = ADTCache.tryLoadFromCache(filename);
	if (adt)
	{
		adt->buildVideoResources();
		if (MultiThread)
			END_MUTEX(adtMutex);

		return adt;
	}

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file && ADTLoader->isALoadableFileExtension(filename))
	{	
		adt = ADTLoader->loadADT(file);
		if (adt)
		{
			adt->buildVideoResources();
			adt->setFileName(filename);
			ADTCache.addToCache(filename, adt);		
		}
	}

	delete file;

	if (MultiThread)
		END_MUTEX(adtMutex);

	return adt;
}

IFileWMO* CD3D9ResourceLoader::loadWMO( const c8* filename )
{
	if (MultiThread)
		BEGIN_MUTEX(wmoMutex);

	IFileWMO* wmo = NULL;
	wmo = WMOCache.tryLoadFromCache(filename);
	if (wmo)
	{
		if (MultiThread)
			END_MUTEX(wmoMutex);

		return wmo;
	}

	MPQFile* file = g_Engine->getWowEnvironment()->openFile(filename);
	if (file && WMOLoader->isALoadableFileExtension(filename))
	{	
		wmo = WMOLoader->loadWMO(file);
		if (wmo)
		{
			wmo->setFileName(filename);
			WMOCache.addToCache(filename, wmo);		
		}
	}

	delete file;

	if (MultiThread)
		END_MUTEX(wmoMutex);

	return wmo;
}

DWORD WINAPI CD3D9ResourceLoader::LoadingThreadFunc( LPVOID lpParam )
{
	SParam* param = (SParam*)lpParam;
	CD3D9ResourceLoader* loader = param->loader;

	DWORD ret = 0;
	STask task;
	while (true)
	{
		bool empty = false;
		::EnterCriticalSection(&loader->cs);
		empty = loader->taskList.empty();
		::LeaveCriticalSection(&loader->cs);
		if (!empty)
		{
			ret = ::WaitForSingleObject(loader->hLoadingEvent, INFINITE);
			if (loader->StopLoading)
				break;
			
			::ResetEvent(loader->hLoadedEvent);
			::ResetEvent(loader->hLoadingStoppedEvent);

			::EnterCriticalSection(&loader->cs);
			empty = loader->taskList.empty();
			if (empty)
			{
				::LeaveCriticalSection(&loader->cs);
				continue;
			}
			task = loader->taskList.front();
			::LeaveCriticalSection(&loader->cs);

			if(task.type == ET_M2)
				task.file = loader->loadM2(task.filename);
			else if (task.type == ET_WMO)
				task.file = loader->loadWMO(task.filename);
			else if (task.type == ET_ADT)
				task.file = loader->loadADT(task.filename);
			else
				_ASSERT(false);

			::EnterCriticalSection(&loader->cs);
			loader->currentTask = task;
			loader->taskList.pop_front();
			::LeaveCriticalSection(&loader->cs);

			::SetEvent(loader->hLoadedEvent);		
			::SetEvent(loader->hLoadingStoppedEvent);
		}
		else
		{
			if (loader->StopLoading)
				break;
			else
				::Sleep(2);
		}
	}

	return 0;
}

void CD3D9ResourceLoader::beginLoadM2( const c8* filename, const SParamBlock& param )
{
	if (!MultiThread || !hasFileExtensionA(filename, "m2"))
		return;

	STask task = {0};
	strcpy_s(task.filename, MAX_PATH, filename);
	task.type = ET_M2;
	task.param = param;

	::EnterCriticalSection(&cs);

	taskList.push_back(task);

	::LeaveCriticalSection(&cs);
}

bool CD3D9ResourceLoader::cancelLoadM2(const c8* filename)
{
	if (!MultiThread || !hasFileExtensionA(filename, "m2"))
		return false;

	stopLoading();

	::EnterCriticalSection(&cs);

	for (T_TaskList::iterator itr = taskList.begin(); itr != taskList.end(); ++itr)
	{
		if ((*itr).type == ET_M2 && _stricmp((*itr).filename, filename) ==0)
		{
			taskList.erase(itr);
			::LeaveCriticalSection(&cs);
			resumeLoading();
			return true;
		}
	}

	::LeaveCriticalSection(&cs);

	resumeLoading();

	return false;
}

void CD3D9ResourceLoader::cancelAll(E_TASK_TYPE type)
{
	if (!MultiThread)
		return;

	stopLoading();

	::EnterCriticalSection(&cs);

	for (T_TaskList::iterator itr = taskList.begin(); itr != taskList.end();)
	{
		if ((*itr).type == type)
			itr = taskList.erase(itr);
		else
			++itr;
	}

	::LeaveCriticalSection(&cs);

	resumeLoading();
}

bool CD3D9ResourceLoader::m2LoadCompleted() const
{
	if (!MultiThread || currentTask.type != ET_M2)
		return false;
	return WAIT_OBJECT_0 == ::WaitForSingleObject(hLoadedEvent, 0);
}

void CD3D9ResourceLoader::beginLoadWMO( const c8* filename, const SParamBlock& param )
{
	if (!MultiThread || !hasFileExtensionA(filename, "wmo"))
		return;

	STask task = {0};
	strcpy_s(task.filename, MAX_PATH, filename);
	task.type = ET_WMO;
	task.param = param;

	::EnterCriticalSection(&cs);

	taskList.push_back(task);

	::LeaveCriticalSection(&cs);
}

bool CD3D9ResourceLoader::cancelLoadWMO( const c8* filename )
{
	if (!MultiThread || !hasFileExtensionA(filename, "wmo"))
		return false;

	stopLoading();

	::EnterCriticalSection(&cs);

	for (T_TaskList::iterator itr = taskList.begin(); itr != taskList.end(); ++itr)
	{
		if ((*itr).type == ET_WMO && _stricmp((*itr).filename, filename) ==0)
		{
			taskList.erase(itr);
			::LeaveCriticalSection(&cs);
			resumeLoading();
			return true;
		}
	}

	::LeaveCriticalSection(&cs);

	resumeLoading();

	return false;
}

bool CD3D9ResourceLoader::wmoLoadCompleted() const
{
	if (!MultiThread || currentTask.type != ET_WMO)
		return false;
	return WAIT_OBJECT_0 == ::WaitForSingleObject(hLoadedEvent, 0);
}

void CD3D9ResourceLoader::beginLoadADT( const c8* filename, const SParamBlock& param )
{
	if (!MultiThread || !hasFileExtensionA(filename, "adt"))
		return;

	STask task = {0};
	strcpy_s(task.filename, MAX_PATH, filename);
	task.type = ET_ADT;
	task.param = param;

	::EnterCriticalSection(&cs);

	taskList.push_back(task);

	::LeaveCriticalSection(&cs);
}

bool CD3D9ResourceLoader::cancelLoadADT( const c8* filename )
{
	if (!MultiThread || !hasFileExtensionA(filename, "adt"))
		return false;

	stopLoading();

	::EnterCriticalSection(&cs);

	for (T_TaskList::iterator itr = taskList.begin(); itr != taskList.end(); ++itr)
	{
		if ((*itr).type == ET_ADT && _stricmp((*itr).filename, filename) ==0)
		{
			taskList.erase(itr);
			::LeaveCriticalSection(&cs);
			resumeLoading();
			return true;
		}
	}

	::LeaveCriticalSection(&cs);

	resumeLoading();

	return false;
}

bool CD3D9ResourceLoader::adtLoadCompleted() const
{
	if (!MultiThread || currentTask.type != ET_ADT)
		return false;
	return WAIT_OBJECT_0 == ::WaitForSingleObject(hLoadedEvent, 0);
}

void CD3D9ResourceLoader::stopLoading()
{
	if(MultiThread)
	{
		::ResetEvent(hLoadingEvent); 
		::WaitForSingleObject(hLoadingStoppedEvent, INFINITE);
	}
}

void CD3D9ResourceLoader::resumeLoading()
{ 
	if(MultiThread)
	{
		::SetEvent(hLoadingEvent); 
	}
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

void CD3D9ResourceLoader::endLoading()
{
	if (MultiThread)
	{
		//thread
		CD3D9ResourceLoader::StopLoading = true;
		::SetEvent(hLoadingEvent);
		if( WAIT_OBJECT_0 != ::WaitForSingleObject(hThread, INFINITE))
		{
			_ASSERT(false);
		}
	}
}

void CD3D9ResourceLoader::clearCurrentTask()
{
	::EnterCriticalSection(&cs);
	memset(&currentTask, 0, sizeof(STask));
	::LeaveCriticalSection(&cs);
}

