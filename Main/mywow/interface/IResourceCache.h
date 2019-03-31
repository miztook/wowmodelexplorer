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
	IReferenceCounted() : ReferenceCounter(1), Cache(nullptr)
	{
		ASSERT(ReferenceCounter.is_lock_free());
	}
	virtual ~IReferenceCounted(){}

	//
	void grab()
	{ 
		++ReferenceCounter;
	}

	bool drop()
	{
		int32_t refCount;

		ASSERT( ReferenceCounter>0 );
		--ReferenceCounter;
		refCount = ReferenceCounter;

		if (refCount == 1 && Cache)			
		{
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
	int32_t getReferenceCount() const 
	{ 
		int32_t refCount;

		refCount = ReferenceCounter;

		return refCount; 
	}

	const char* getFileName() const { return FileName.c_str(); }
	void setFileName(const char* filename) 
	{
		ASSERT(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));
		FileName = filename; 
	}
	IResourceCache<T>* getCache() const { return Cache; }
	void setCache(IResourceCache<T>* cache) { Cache = cache; }

private:
	string256	FileName;
	atomic_type<int> ReferenceCounter;
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
	void setCacheLimit(uint32_t limit);
	uint32_t getCacheLimit() const { return CacheLimit; }

protected:
	typedef std::list<T*, qzone_allocator<T*>  > T_FreeList;
	T_FreeList FreeList;			//不在使用中，可以删除
	
#ifdef USE_QALLOCATOR
	typedef std::map<string256, T*, std::less<string256>, qzone_allocator<std::pair<string256, T*>>>	T_UseMap;
#else
	typedef std::unordered_map<string256, T*, string256::string_hash>	T_UseMap;
#endif

	T_UseMap UseMap;

	volatile uint32_t CacheLimit;		//空闲列表大小
	lock_type cs;
};

template <class T>
void IResourceCache<T>::setCacheLimit( uint32_t limit )
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

	auto itrUse = UseMap.find(filename);
	if (itrUse != UseMap.end())
	{
		itrUse->second->grab();
		T* t = itrUse->second;
		END_LOCK(&cs);
		return t;
	}

	// free cache 中查找
	for ( auto itr = FreeList.begin(); itr != FreeList.end(); ++itr )
	{
		T* t = (*itr);
		const char* fname = t->getFileName();
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
	return nullptr;
}

template <class T>
void IResourceCache<T>::addToCache(T* item)
{
	const char* filename = item->getFileName();
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
	const char* filename = item->getFileName();
	ASSERT(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));
	
	BEGIN_LOCK(&cs);
	auto itr = UseMap.find(filename);
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
			char tmp[512];
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
