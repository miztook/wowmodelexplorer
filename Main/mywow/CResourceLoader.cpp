#include "stdafx.h"
#include "CResourceLoader.h"
#include "mywow.h"
#include "CMemFile.h"
#include "CFileM2.h"
#include "CBLPImage.h"
#include "CPVRImage.h"
#include "CKTXImage.h"
#include "CFileWDT.h"
#include "CFileADT.h"
#include "CFileWMO.h"

CResourceLoader::CResourceLoader()
	: MultiThread(false), StopLoading(false), Suspended(true)
{
	M2Cache_Character.setCacheLimit(10);
	M2Cache_Item.setCacheLimit(10);
	M2Cache_Creature.setCacheLimit(5);
	M2Cache_Particles.setCacheLimit(5);
	M2Cache_Spells.setCacheLimit(5);
	M2Cache_Interface.setCacheLimit(0);
	M2Cache_World.setCacheLimit(10);
	M2Cache_Default.setCacheLimit(0);

	ImageCache.setCacheLimit(0);
	BlpImageCache.setCacheLimit(0);
	PvrImageCache.setCacheLimit(0);
	KtxImageCache.setCacheLimit(0);
	TextureCache.setCacheLimit(0);

	ADTCache.setCacheLimit(5);
	WMOCache.setCacheLimit(0);

	currentTask.file = NULL_PTR;

	INIT_LOCK(&cs);
	INIT_LOCK(&m2CS);
	INIT_LOCK(&imageCS);
	INIT_LOCK(&blpCS);
	INIT_LOCK(&pvrCS);
	INIT_LOCK(&ktxCS);
	INIT_LOCK(&textureCS);
	INIT_LOCK(&wdtCS);
	INIT_LOCK(&adtCS);
	INIT_LOCK(&wmoCS);

	//thread
	INIT_EVENT(&hLoadingEvent, "LoadingEvent"); 

	ThreadParam.loader = this;

	INIT_THREAD(&hThread, LoadingThreadFunc, &ThreadParam, true);

	Suspended = true;

	//::SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
}

CResourceLoader::~CResourceLoader()
{
	DESTROY_LOCK(&wmoCS);
	DESTROY_LOCK(&adtCS);
	DESTROY_LOCK(&wdtCS);
	DESTROY_LOCK(&textureCS);
	DESTROY_LOCK(&ktxCS);
	DESTROY_LOCK(&pvrCS);
	DESTROY_LOCK(&blpCS);
	DESTROY_LOCK(&imageCS);
	DESTROY_LOCK(&m2CS);
	DESTROY_LOCK(&cs); 

	ASSERT(!currentTask.file);

	DESTROY_THREAD(&hThread);
	DESTROY_EVENT(&hLoadingEvent);

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

	TextureCache.flushCache();
	KtxImageCache.flushCache();
	PvrImageCache.flushCache();
	BlpImageCache.flushCache();
	ImageCache.flushCache();

}

IImage* CResourceLoader::loadJPGAsImage( const c8* filename )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&imageCS);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(realfilename, true);
	u32 size = rfile->getSize();
	if (size <= 1)
	{
		if (MultiThread)
			END_LOCK(&imageCS);

		delete rfile;
		return NULL_PTR;
	}

	u8* buffer = (u8*)Z_AllocateTempMemory(size);
	u32 readSize = rfile->read(buffer, size);
	ASSERT(readSize == size);
	delete rfile;

	IImage* image = NULL_PTR;

	IMemFile* memFile = new CMemFile(buffer, size, realfilename, true);

	if (JpgLoader.isALoadableFileExtension(realfilename))
	{	
		image = JpgLoader.loadAsImage(memFile);
	}

	delete memFile;

	if (MultiThread)
		END_LOCK(&imageCS);

	return image;
}

IImage* CResourceLoader::loadPNGAsImage(const c8* filename)
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&imageCS);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(realfilename, true);
	u32 size = rfile->getSize();
	if (size <= 1)
	{
		if (MultiThread)
			END_LOCK(&imageCS);

		delete rfile;
		return NULL_PTR;
	}

	u8* buffer = (u8*)Z_AllocateTempMemory(size);
	u32 readSize = rfile->read(buffer, size);
	ASSERT(readSize == size);
	delete rfile;

	IImage* image = NULL_PTR;

	IMemFile* memFile = new CMemFile(buffer, size, realfilename, true);

	if (PngLoader.isALoadableFileExtension(realfilename))
	{	
		image = PngLoader.loadAsImage(memFile);
	}

	delete memFile;

	if (MultiThread)
		END_LOCK(&imageCS);

	return image;
}

IImage* CResourceLoader::loadBLPAsImage( const c8* filename, bool changeRB)
{	
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&imageCS);

	IImage* image = ImageCache.tryLoadFromCache(realfilename);
	if (image)
	{
		if (MultiThread)
			END_LOCK(&imageCS);
		return image;
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && BlpLoader.isALoadableFileExtension(realfilename))
	{	
		image = BlpLoader.loadAsImage(file, changeRB);
		if (image)
		{
			image->setFileName(realfilename);
			ImageCache.addToCache(image);		
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&imageCS);

	return image;
}

IBLPImage* CResourceLoader::loadBLP( const c8* filename )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&blpCS);

	IBLPImage* image = BlpImageCache.tryLoadFromCache(realfilename);
	if (image)
	{
		if (MultiThread)
			END_LOCK(&blpCS);
		return image;
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && BlpLoader.isALoadableFileExtension(realfilename))
	{	
		image = new CBLPImage();
		if (image->loadFile(file, !g_Engine->isDXFamily()))
		{
			image->setFileName(realfilename);
			BlpImageCache.addToCache(image);		
		}
		else
		{
			image->drop();
			image = NULL_PTR;
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&blpCS);

	return image;
}

IImage* CResourceLoader::loadPVRAsImage( const c8* filename )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&imageCS);

	IImage* image = ImageCache.tryLoadFromCache(realfilename);
	if (image)
	{
		if (MultiThread)
			END_LOCK(&imageCS);
		return image;
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && PvrLoader.isALoadableFileExtension(realfilename))
	{	
		image = PvrLoader.loadAsImage(file);
		if (image)
		{
			image->setFileName(realfilename);
			ImageCache.addToCache(image);		
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&imageCS);

	return image;
}

IPVRImage* CResourceLoader::loadPVR( const c8* filename )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&pvrCS);

	IPVRImage* image = PvrImageCache.tryLoadFromCache(realfilename);
	if (image)
	{
		if (MultiThread)
			END_LOCK(&pvrCS);
		return image;
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && PvrLoader.isALoadableFileExtension(realfilename))
	{	
		image = new CPVRImage();
		if (image->loadFile(file))
		{
			image->setFileName(realfilename);
			PvrImageCache.addToCache(image);		
		}
		else
		{
			image->drop();
			image = NULL_PTR;
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&pvrCS);

	return image;
}

IImage* CResourceLoader::loadKTXAsImage( const c8* filename )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&imageCS);

	IImage* image = ImageCache.tryLoadFromCache(realfilename);
	if (image)
	{
		if (MultiThread)
			END_LOCK(&imageCS);
		return image;
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && KtxLoader.isALoadableFileExtension(realfilename))
	{	
		image = KtxLoader.loadAsImage(file);
		if (image)
		{
			image->setFileName(realfilename);
			ImageCache.addToCache(image);		
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&imageCS);

	return image;
}

IKTXImage* CResourceLoader::loadKTX( const c8* filename )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&ktxCS);

	IKTXImage* image = KtxImageCache.tryLoadFromCache(realfilename);
	if (image)
	{
		if (MultiThread)
			END_LOCK(&ktxCS);
		return image;
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && KtxLoader.isALoadableFileExtension(realfilename))
	{	
		image = new CKTXImage;
		if (image->loadFile(file))
		{
			image->setFileName(realfilename);
			KtxImageCache.addToCache(image);		
		}
		else
		{
			image->drop();
			image = NULL_PTR;
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&ktxCS);

	return image;
}

IFileM2* CResourceLoader::loadM2( const c8* filename, bool videobuild )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&m2CS);

	IFileM2* m2 = NULL_PTR;
	IResourceCache<IFileM2>* cache = getM2Cache(realfilename);

	if (cache)
	{
		m2 = cache->tryLoadFromCache(realfilename);
		if (m2)
		{
			if (videobuild)
				m2->buildVideoResources();

			if (MultiThread)
				END_LOCK(&m2CS);

			return m2;
		}
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && M2Loader.isALoadableFileExtension(realfilename))
	{	
		m2 = M2Loader.loadM2(file);
		if(m2 && cache)
		{
			m2->setFileName(realfilename);

			if (videobuild)
				m2->buildVideoResources();

			cache->addToCache(m2);
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&m2CS);

	//callback
	if (m2)
	{
		for (T_M2LoadedList::const_iterator itr = M2LoadedCallbackList.begin(); itr != M2LoadedCallbackList.end(); ++itr)
		{
			(*itr)->onM2Loaded(m2);
		}
	}
	
	return m2;
}

IFileWDT* CResourceLoader::loadWDT( const c8* filename, s32 mapid, bool simple )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&wdtCS);

	IFileWDT* wdt = NULL_PTR;

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && WDTLoader.isALoadableFileExtension(realfilename))	
		wdt = WDTLoader.loadWDT(file, mapid, simple);

	delete file;

	if (MultiThread)
		END_LOCK(&wdtCS);

	return wdt;
}

IFileADT* CResourceLoader::loadADT( const c8* filename, bool simple, bool videobuild )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&adtCS);

	IFileADT* adt = NULL_PTR;

	//simple情况下不加入缓存
	if (!simple)
	{
		adt = ADTCache.tryLoadFromCache(realfilename);
		if (adt)
		{
			if (videobuild)
				adt->buildVideoResources();

			if (MultiThread)
				END_LOCK(&adtCS);

			return adt;
		}
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && ADTLoader.isALoadableFileExtension(realfilename))
	{	
		adt = ADTLoader.loadADT(file, simple);
		if (adt)
		{
			if (simple)
			{
				adt->setFileName(realfilename);
			}
			else
			{
				adt->setFileName(realfilename);

				if (videobuild)
					adt->buildVideoResources();

				ADTCache.addToCache(adt);	
			}	
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&adtCS);

	return adt;
}

IFileADT* CResourceLoader::loadADTTextures(const c8* filename)
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&adtCS);

	IFileADT* adt = NULL_PTR;

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && ADTLoader.isALoadableFileExtension(realfilename))
	{	
		adt = ADTLoader.loadADTTextures(file);
		if (adt)
		{
			adt->setFileName(realfilename);
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&adtCS);

	return adt;
}

IFileWMO* CResourceLoader::loadWMO( const c8* filename, bool videobuild )
{
	if (strlen(filename) == 0)
		return NULL_PTR;

	c8 realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (MultiThread)
		BEGIN_LOCK(&wmoCS);

	IFileWMO* wmo = WMOCache.tryLoadFromCache(realfilename);
	if (wmo)
	{
		if (videobuild)
			wmo->buildVideoResources();

		if (MultiThread)
			END_LOCK(&wmoCS);

		return wmo;
	}

	IMemFile* file = g_Engine->getWowEnvironment()->openFile(realfilename);
	if (file && WMOLoader.isALoadableFileExtension(realfilename))
	{	
		wmo = WMOLoader.loadWMO(file);
		if (wmo)
		{
			wmo->setFileName(realfilename);

			if (videobuild)
				wmo->buildVideoResources();

			WMOCache.addToCache(wmo);		
		}
	}

	delete file;

	if (MultiThread)
		END_LOCK(&wmoCS);

	return wmo;
}

int CResourceLoader::LoadingThreadFunc( void* lpParam )
{
	SParam* param = (SParam*)lpParam;
	CResourceLoader* loader = param->loader;

#ifdef MW_VIDEO_MULTITHREAD
		bool videobuild = true;
#else
		bool videobuild = false;
#endif

	STask task;	
	bool stop = false;
	while (!stop)
	{
		BEGIN_LOCK(&loader->cs);
		//check stop
		stop = loader->StopLoading;			//请求退出的信号
		if (stop)
		{
			loader->Suspended = true;
			END_LOCK(&loader->cs);
			break;
		}

		bool bEmpty = true;		
		while(!loader->taskList.empty())		//任务列表是否为空
		{
			task = loader->taskList.front();
			if(task.cancel)
			{
				loader->taskList.pop_front();
			}
			else
			{	
				bEmpty = false;
				break;
			}
		}
		END_LOCK(&loader->cs);
		
		if (!bEmpty)
		{
			WAIT_EVENT(&loader->hLoadingEvent);				//等待可以loading事件

			//在等待事件后，stop可能发生变化，需要重新判断
			BEGIN_LOCK(&loader->cs);
			stop = loader->StopLoading;			
			if (stop)
			{
				loader->Suspended = true;
				END_LOCK(&loader->cs);
				break;
			}
			END_LOCK(&loader->cs);

			//在等待事件后，tasklist可能发生变化，需要重新判断
			BEGIN_LOCK(&loader->cs);
			while(!loader->taskList.empty())		//任务列表是否为空
			{
				task = loader->taskList.front();
				loader->taskList.pop_front();

				if(!task.cancel)
				{	
					bEmpty = false;
					break;
				}
			}
			END_LOCK(&loader->cs);

			if (!bEmpty)
			{
				BEGIN_LOCK(&loader->cs);
				loader->Suspended = false;
				END_LOCK(&loader->cs);

				void* file = NULL_PTR;
				if(task.type == ET_M2)
					file = loader->loadM2(task.filename, videobuild);
				else if (task.type == ET_WMO)
					file = loader->loadWMO(task.filename, videobuild);
				else if (task.type == ET_ADT)
					file = loader->loadADT(task.filename, false, videobuild);
				else
					ASSERT(false);

				BEGIN_LOCK(&loader->cs);
				task.file = file;
				loader->currentTask = task;
				loader->Suspended = true;
				END_LOCK(&loader->cs);
			}
		}
		else
		{
			BEGIN_LOCK(&loader->cs);
			loader->Suspended = true;
			END_LOCK(&loader->cs);

			SLEEP(1);		
		}
	}

	return 0;
}

void CResourceLoader::beginLoadM2( const c8* filename, const SParamBlock& param )
{
	if (!MultiThread || !hasFileExtensionA(filename, "m2"))
		return;

	STask task = {0};
	Q_strcpy(task.filename, QMAX_PATH, filename);
	normalizeFileName(task.filename);
	Q_strlwr(task.filename);
	task.type = ET_M2;
	task.param = param;

	BEGIN_LOCK(&cs);

	taskList.emplace_back(task);

	END_LOCK(&cs);
}

void CResourceLoader::cancelAll(E_TASK_TYPE type)
{
	if (!MultiThread)
		return;

	BEGIN_LOCK(&cs);

	for (T_TaskList::iterator itr = taskList.begin(); itr != taskList.end(); ++itr)
	{
		if ((*itr).type == type)
			itr->cancel = true;
	}

	if (currentTask.type == type)
	{
		clearLoadedFiles();
	}

	END_LOCK(&cs);
}

bool CResourceLoader::m2LoadCompleted()
{
	E_TASK_TYPE type;
	BEGIN_LOCK(&cs);
	type = currentTask.type;
	END_LOCK(&cs);

	if (!MultiThread || type != ET_M2)
		return false;
	return true;
}

void CResourceLoader::beginLoadWMO( const c8* filename, const SParamBlock& param )
{
	if (!MultiThread || !hasFileExtensionA(filename, "wmo"))
		return;

	STask task = {0};
	Q_strcpy(task.filename, QMAX_PATH, filename);
	normalizeFileName(task.filename);
	Q_strlwr(task.filename);
	task.type = ET_WMO;
	task.param = param;

	BEGIN_LOCK(&cs);

	taskList.emplace_back(task);

	END_LOCK(&cs);
}

bool CResourceLoader::wmoLoadCompleted()
{
	E_TASK_TYPE type;
	BEGIN_LOCK(&cs);
	type = currentTask.type;
	END_LOCK(&cs);

	if (!MultiThread || type != ET_WMO)
		return false;
	return true;
}

void CResourceLoader::beginLoadADT( const c8* filename, const SParamBlock& param )
{
	if (!MultiThread || !hasFileExtensionA(filename, "adt"))
		return;

	STask task = {0};
	Q_strcpy(task.filename, QMAX_PATH, filename);
	normalizeFileName(task.filename);
	Q_strlwr(task.filename);
	task.type = ET_ADT;
	task.param = param;

	BEGIN_LOCK(&cs);

	taskList.emplace_back(task);

	END_LOCK(&cs);
}

bool CResourceLoader::adtLoadCompleted()
{
	E_TASK_TYPE type;
	BEGIN_LOCK(&cs);
	type = currentTask.type;
	END_LOCK(&cs);

	if (!MultiThread || type != ET_ADT)
		return false;
	return true;
}

void CResourceLoader::waitLoadingSuspend()
{
	if(MultiThread)
	{
		bool bSuspend = false;
		BEGIN_LOCK(&cs);
		bSuspend = Suspended;
		END_LOCK(&cs);

		while (!bSuspend)		//确保加载停止
		{
			SLEEP(10);

			BEGIN_LOCK(&cs);
			bSuspend = Suspended;
			END_LOCK(&cs);
		}
	}
}

void CResourceLoader::resumeLoading()
{ 
	if(MultiThread)
	{
		SET_EVENT(&hLoadingEvent); 
		SLEEP(1);
	}
}

IResourceCache<IFileM2>* CResourceLoader::getM2Cache( const c8* filename )
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
	case MT_PARTICLES:
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

void CResourceLoader::beginLoading()
{
	RESUME_THREAD(&hThread);
	SET_EVENT(&hLoadingEvent);
	MultiThread = true;
}

void CResourceLoader::endLoading()
{
	if (MultiThread)
	{
		waitLoadingSuspend();

		//thread
		BEGIN_LOCK(&cs);
		//取消所有任务
		for (T_TaskList::iterator itr = taskList.begin(); itr != taskList.end(); ++itr)
		{
			itr->cancel = true;
		}

		CResourceLoader::StopLoading = true;			//请求结束
		END_LOCK(&cs);

		clearLoadedFiles();

		resumeLoading();		//继续执行完loading线程, 这时仍然可能有最后一个task已经在加载

		waitLoadingSuspend();		//等待所有task停止, 清除最后的资源

		clearLoadedFiles();

		if( !WAIT_THREAD(&hThread) )
		{
			ASSERT(false);
		}

		MultiThread = false;
	}
}

void CResourceLoader::clearCurrentTask()
{
	BEGIN_LOCK(&cs);
	memset(&currentTask, 0, sizeof(STask));
	END_LOCK(&cs);
}

void CResourceLoader::setCacheLimit( E_CACHE_TYPE type, u32 limit )
{
	switch(type)
	{
	case ECT_M2_CHARACTER:
		M2Cache_Character.setCacheLimit(limit);
		break;
	case ECT_M2_ITEM:
		M2Cache_Item.setCacheLimit(limit);
		break;
	case ECT_M2_CREATURE:
		M2Cache_Creature.setCacheLimit(limit);
		break;
	case ECT_M2_PARTICLES:
		M2Cache_Particles.setCacheLimit(limit);
		break;
	case ECT_M2_SPELLS:
		M2Cache_Spells.setCacheLimit(limit);
		break;
	case ECT_M2_INTERFACE:
		M2Cache_Interface.setCacheLimit(limit);
		break;
	case ECT_M2_WORLD:
		M2Cache_World.setCacheLimit(limit);
		break;
	case ECT_M2_DEFAULT:
		M2Cache_Default.setCacheLimit(limit);
		break;
	case ECT_IMAGE:
		ImageCache.setCacheLimit(limit);
		break;
	case ECT_BLP_IMAGE:
		BlpImageCache.setCacheLimit(limit);
		break;
	case ECT_PVR_IMAGE:
		PvrImageCache.setCacheLimit(limit);
		break;
	case ECT_KTX_IMAGE:
		KtxImageCache.setCacheLimit(limit);
		break;
	case ECT_TEXTURE:
		TextureCache.setCacheLimit(limit);
		break;
	case ECT_ADT:
		ADTCache.setCacheLimit(limit);
		break;
	case ECT_WMO:
		WMOCache.setCacheLimit(limit);
		break;
	default:
		ASSERT(false);
		break;
	}
}

u32 CResourceLoader::getCacheLimit( E_CACHE_TYPE type ) const
{
	switch(type)
	{
	case ECT_M2_CHARACTER:
		return M2Cache_Character.getCacheLimit();
	case ECT_M2_ITEM:
		return M2Cache_Item.getCacheLimit();
	case ECT_M2_CREATURE:
		return M2Cache_Creature.getCacheLimit();
	case ECT_M2_PARTICLES:
		return M2Cache_Particles.getCacheLimit();
	case ECT_M2_SPELLS:
		return M2Cache_Spells.getCacheLimit();
	case ECT_M2_INTERFACE:
		return M2Cache_Interface.getCacheLimit();
	case ECT_M2_WORLD:
		return M2Cache_World.getCacheLimit();
	case ECT_M2_DEFAULT:
		return M2Cache_Default.getCacheLimit();
	case ECT_IMAGE:
		return ImageCache.getCacheLimit();
	case ECT_BLP_IMAGE:
		return BlpImageCache.getCacheLimit();
	case ECT_PVR_IMAGE:
		return PvrImageCache.getCacheLimit();
	case ECT_KTX_IMAGE:
		return KtxImageCache.getCacheLimit();
	case ECT_TEXTURE:
		return TextureCache.getCacheLimit();
	case ECT_ADT:
		return ADTCache.getCacheLimit();
	case ECT_WMO:
		return WMOCache.getCacheLimit();
	default:
		ASSERT(false);
		return 0;
	}
}

void CResourceLoader::registerM2Loaded( IM2LoadCallback* callback )
{
	if (std::find(M2LoadedCallbackList.begin(), M2LoadedCallbackList.end(), callback) == M2LoadedCallbackList.end())
	{
		M2LoadedCallbackList.push_back(callback);
	}
}

void CResourceLoader::removeM2Loaded( IM2LoadCallback* callback )
{
	M2LoadedCallbackList.remove(callback);
}

void CResourceLoader::clearLoadedFiles()
{
	if (!currentTask.file)
		return;

	switch(currentTask.type)
	{
	case ET_M2:
		{
			IReferenceCounted<IFileM2>* iref = (IReferenceCounted<IFileM2>*)currentTask.file;
			iref->drop();
		}
		break;
	case ET_WMO:
		{
			IReferenceCounted<IFileWMO>* iref = (IReferenceCounted<IFileWMO>*)currentTask.file;
			iref->drop();
		}
		break;
	case ET_ADT:
		{
			IReferenceCounted<IFileADT>* iref = (IReferenceCounted<IFileADT>*)currentTask.file;
			iref->drop();
		}
		break;
	default:
		ASSERT(false);
		break;
	}		

	memset(&currentTask, 0, sizeof(currentTask));
}

