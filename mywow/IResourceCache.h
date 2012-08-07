#pragma once

#include "core.h"
#include <list>
#include <map>

//T是缓存的类型

template <class T>
class IResourceCache;

extern CRITICAL_SECTION g_refCS;

template <class T>
class IReferenceCounted
{
public:
	//
	IReferenceCounted() : ReferenceCounter(1), Cache(NULL) {}
	virtual ~IReferenceCounted(){}

	//
	void grab()
	{ 
		::EnterCriticalSection(&g_refCS);
		++ReferenceCounter;
		::LeaveCriticalSection(&g_refCS);
	}

	bool drop()
	{
		s32 refCount;
		::EnterCriticalSection(&g_refCS);

		_ASSERT( ReferenceCounter>0 );
		--ReferenceCounter;

		refCount = ReferenceCounter;
		::LeaveCriticalSection(&g_refCS);

		if (refCount == 1 && Cache)
		{
			onRemove();
			Cache->removeFromCache(static_cast<T*>(this));
		}
		else if ( 0 == refCount )
		{
			delete this;
			return true;
		}
		return false;
	}

	//
	s32 getReferenceCount() const { return ReferenceCounter; }
	const c8* getFileName() const { return fileName; }
	void setFileName(const c8* filename) { strcpy_s(fileName, MAX_PATH, filename); }

	IResourceCache<T>* Cache;	

protected:
	//对于有显存资源的类(ITexture, IFileM2等)，为了优化多线程加载和节省显存，加载线程只加载内存资源，主线程负责构造显存资源
	virtual void onRemove() = 0;

	volatile s32 ReferenceCounter;

private:
	c8		fileName[MAX_PATH];
};

template <class T>			
class IResourceCache
{
public:
	IResourceCache() : CacheLimit(10) { }
	virtual ~IResourceCache() {}

public:
	T* tryLoadFromCache( const char* filename );
	void addToCache( const char* filename, T* item );
	void removeFromCache( T* item );
	void flushCache();
	void setCacheLimit(u32 limit) { CacheLimit = limit; }

protected:
	typedef std::list<T*, qzone_allocator<T*>> T_FreeList;

	T_FreeList FreeList;			//不在使用中，可以删除
	u32 CacheLimit;		//空闲列表大小

	typedef std::map<string<MAX_PATH>, T*, std::less<string<MAX_PATH>>, qzone_allocator<std::pair<string<MAX_PATH>, T*>>>	T_UseMap;
	T_UseMap UseMap;
};

template <class T>
T* IResourceCache<T>::tryLoadFromCache( const char* filename )
{
	T_UseMap::iterator itr = UseMap.find(filename);
	if (itr != UseMap.end())
	{
		itr->second->grab();
		return itr->second;
	}

	// free cache 中查找
	for ( T_FreeList::iterator itr = FreeList.begin(); itr != FreeList.end(); ++itr )
	{
		T* t = (*itr);
		const c8* fname = t->getFileName();
		if ( _stricmp(fname, filename) == 0 )			//找到，移到use cache
		{
			t->grab();
			UseMap[filename] = t;
			FreeList.erase(itr);		
			return t;
		}
	}
	return NULL;
}

template <class T>
void IResourceCache<T>::addToCache( const char* filename, T* item)
{
	UseMap[filename] = item;
	item->grab();		
	item->Cache = this;
}

template <class T>
void IResourceCache<T>::removeFromCache( T* item )
{
	const c8* filename = item->getFileName();
	T_UseMap::iterator itr = UseMap.find(filename);
	_ASSERT(itr != UseMap.end());
	if (itr != UseMap.end())
		UseMap.erase(itr);

	while( FreeList.size() > CacheLimit )
	{
		_ASSERT(FreeList.back()->getReferenceCount() == 1);
		FreeList.back()->drop();
		FreeList.pop_back();
	}
	FreeList.push_front(item);
}

template <class T>
void IResourceCache<T>::flushCache()
{
	for(T_UseMap::iterator itr = UseMap.begin(); itr != UseMap.end();)
	{
		T* t = itr->second;
		_ASSERT(t->getReferenceCount() == 1);
		t->drop();
		itr = UseMap.erase(itr);
	}

	while( FreeList.size() > 0 )
	{
		_ASSERT(FreeList.back()->getReferenceCount() == 1);
		FreeList.back()->drop();
		FreeList.pop_back();
	}
}
