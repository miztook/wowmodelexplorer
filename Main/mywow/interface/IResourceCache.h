#pragma once

#include "core.h"
#include <list>
#include <unordered_map>
#include <map>
#include "CSysGlobal.h"
#include "CSysSync.h"
#include "CSysUtility.h"

//T是缓存的类型

template <class T>
class IResourceCache;

template <class T>
class IReferenceCounted
{
public:
	//
	IReferenceCounted() : ReferenceCounter(1), Cache(NULL_PTR)
	{
		
	}
	virtual ~IReferenceCounted(){}

	//
	void grab()
	{ 
		BEGIN_LOCK(&g_Globals.refCS);
		++ReferenceCounter;
		END_LOCK(&g_Globals.refCS);
	}

	bool drop()
	{
		s32 refCount;

		BEGIN_LOCK(&g_Globals.refCS);

		ASSERT( ReferenceCounter>0 );
		--ReferenceCounter;
		refCount = ReferenceCounter;

		END_LOCK(&g_Globals.refCS);

		if (refCount == 1 && Cache)			
		{
			onRemove();
			Cache->removeFromCache(static_cast<T*>(this));	
		}
		else if ( 0 == refCount )
		{
			if (!Cache)
				onRemove();

			delete this;
			return true;
		}
		return false;
	}

	//
	s32 getReferenceCount() const 
	{ 
		s32 refCount;

		BEGIN_LOCK(&g_Globals.refCS);
		refCount = ReferenceCounter;
		END_LOCK(&g_Globals.refCS);

		return refCount; 
	}

	const c8* getFileName() const { return FileName.c_str(); }
	void setFileName(const c8* filename) 
	{
		ASSERT(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));
		FileName = filename; 
	}
	IResourceCache<T>* getCache() const { return Cache; }
	void setCache(IResourceCache<T>* cache) { Cache = cache; }

protected:
	virtual void onRemove() = 0;

private:
	string256	FileName;
	volatile s32 ReferenceCounter;
	IResourceCache<T>* Cache;	
};

template <class T>			
class IResourceCache
{
private:
	DISALLOW_COPY_AND_ASSIGN(IResourceCache);

public:
	IResourceCache() : CacheLimit(10) { INIT_LOCK(&cs); }
	virtual ~IResourceCache() { DESTROY_LOCK(&cs); }

public:
	T* tryLoadFromCache( const char* filename );
	void addToCache( T* item );
	void removeFromCache( T* item );
	void flushCache();
	void setCacheLimit(u32 limit);
	u32 getCacheLimit() const { return CacheLimit; }

protected:
	typedef std::list<T*, qzone_allocator<T*>  > T_FreeList;
	T_FreeList FreeList;			//不在使用中，可以删除
	
#ifdef USE_QALLOCATOR
	typedef std::map<string256, T*, std::less<string256>, qzone_allocator<std::pair<string256, T*>>>	T_UseMap;
#else
	typedef std::unordered_map<string256, T*, string256::string_hash>	T_UseMap;
#endif

	T_UseMap UseMap;

	volatile u32 CacheLimit;		//空闲列表大小
	lock_type cs;
};

template <class T>
void IResourceCache<T>::setCacheLimit( u32 limit )
{
	BEGIN_LOCK(&cs);
	CacheLimit = limit;
	END_LOCK(&cs);
}

template <class T>
T* IResourceCache<T>::tryLoadFromCache( const char* filename )
{
	ASSERT(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));

	BEGIN_LOCK(&cs);

	typename T_UseMap::const_iterator itrUse = UseMap.find(filename);
	if (itrUse != UseMap.end())
	{
		itrUse->second->grab();
		T* t = itrUse->second;
		END_LOCK(&cs);
		return t;
	}

	// free cache 中查找
	for ( typename T_FreeList::iterator itr = FreeList.begin(); itr != FreeList.end(); ++itr )
	{
		T* t = (*itr);
		const c8* fname = t->getFileName();
		if ( Q_stricmp(fname, filename) == 0 )			//找到，移到use cache
		{
			t->grab();
			UseMap[filename] = t;
			FreeList.erase(itr);		

			END_LOCK(&cs);
			return t;
		}
	}

	END_LOCK(&cs);
	return NULL_PTR;
}

template <class T>
void IResourceCache<T>::addToCache(T* item)
{
	const c8* filename = item->getFileName();
	ASSERT(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));

	BEGIN_LOCK(&cs);

	ASSERT(UseMap.find(filename) == UseMap.end());
	UseMap[filename] = item;
	item->grab();		
	item->setCache(this);

	END_LOCK(&cs);
}

template <class T>
void IResourceCache<T>::removeFromCache( T* item )
{
	const c8* filename = item->getFileName();
	ASSERT(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));
	
	BEGIN_LOCK(&cs);
	typename T_UseMap::iterator itr = UseMap.find(filename);
	ASSERT(itr != UseMap.end());
	if (itr != UseMap.end())
		UseMap.erase(itr);

	if (!CacheLimit)			//不会移到freelist
	{
		END_LOCK(&cs);

		item->drop();
		return;
	}

	while( FreeList.size() >= CacheLimit )
	{
		ASSERT(FreeList.back()->getReferenceCount() == 1);
		T* t = FreeList.back();
		END_LOCK(&cs);

		t->drop();
		
		BEGIN_LOCK(&cs);
		FreeList.pop_back();
	}
	FreeList.push_front(item);

	END_LOCK(&cs);
}

template <class T>
void IResourceCache<T>::flushCache()
{
	BEGIN_LOCK(&cs);

	for(typename T_UseMap::const_iterator itr = UseMap.begin(); itr != UseMap.end(); ++itr)
	{
		T* t = itr->second;
		ASSERT(t->getReferenceCount() == 1);
		if (t->getReferenceCount() > 1)
		{
			c8 tmp[512];
			Q_sprintf(tmp, 512, "Resource Cache Leaked! %s", itr->first.c_str());
			CSysUtility::messageBoxWarning(tmp);
		}
		END_LOCK(&cs);

		t->drop();

		BEGIN_LOCK(&cs);
	}

	while( !FreeList.empty() )
	{
		T* t = FreeList.back();
		ASSERT(t->getReferenceCount() == 1);
		if (t->getReferenceCount() > 1)
		{
			CSysUtility::messageBoxWarning("Resource Cache Leaked!");
		}

		END_LOCK(&cs);

		t->drop();

		BEGIN_LOCK(&cs);

		FreeList.pop_back();
	}

	END_LOCK(&cs);
}
