#pragma once

#include "base.h"

//分配定量的资源，空闲链表和使用链表
template<typename T>
class IResourcePool
{
private:
	DISALLOW_COPY_AND_ASSIGN(IResourcePool);

public:
	IResourcePool() : Quota(0), FreeCount(0), Entries(nullptr)	{ InitializeListHead(&FreeList); }
	virtual ~IResourcePool() 
	{ 
		if (Entries)
		{
			delete[] Entries;
		}

		InitializeListHead(&FreeList);
		FreeCount = Quota;
	}

public:
	void allocateAll(uint32_t quota);

	T* get();
	void put(T* t);

	uint32_t	getUsedSize() const { return Quota - FreeCount; }

private:
	struct SEntry
	{
		LENTRY	Link;
		T	Data;
	};

private:

	uint32_t		Quota;
	uint32_t		FreeCount;
	SEntry*		Entries;
	LENTRY		FreeList;
};

template<typename T>
void IResourcePool<T>::put( T* t )
{
	SEntry* entry = CONTAINING_RECORD(t, SEntry, Data);
	InsertTailList(&FreeList, &entry->Link);	

	++FreeCount;
	ASSERT(FreeCount <= Quota);
}

template<typename T>
T* IResourcePool<T>::get()
{
	if (IsListEmpty(&FreeList))
		return nullptr;

	LENTRY* p = RemoveHeadList(&FreeList);
	SEntry* entry = reinterpret_cast<SEntry*>CONTAINING_RECORD(p, SEntry, Link);
	--FreeCount;

	return &entry->Data;
}

template<typename T>
void IResourcePool<T>::allocateAll( uint32_t quota )
{
	if(Entries)
		return;

	Quota = quota;

	Entries = new SEntry[Quota];

	for (uint32_t i=0; i<Quota; ++i)
	{
		InsertTailList(&FreeList, &Entries[i].Link);
	}

	FreeCount = Quota;
}