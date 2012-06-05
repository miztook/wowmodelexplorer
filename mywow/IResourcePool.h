#pragma once

#include "base.h"
#include <list>

//分配定量的资源，空闲链表和使用链表
template<typename T>
class IResourcePool
{
public:
	IResourcePool() : Quota(0), FreeCount(0), Entries(NULL)	{ InitializeListHead(&FreeList); }
	virtual ~IResourcePool() 
	{ 
		if (Entries)
		{
			if (Temp)
				Hunk_FreeTempMemory(Entries);
			else
				delete[] Entries;
		}

		InitializeListHead(&FreeList);
		FreeCount = Quota;
	}

public:
	void allocateAll(u32 quota, bool temp);

	T* get();
	void put(T* t);

	u32	getUsedSize() const { return Quota - FreeCount; }

private:
	struct SEntry
	{
		T	Data;
		LENTRY	Link;
	};

private:
	bool	Temp;

	u32		Quota;
	SEntry*		Entries;

	u32		FreeCount;
	LENTRY		FreeList;
};

template<typename T>
void IResourcePool<T>::put( T* t )
{
	SEntry* entry = CONTAINING_RECORD(t, SEntry, Data);
	InsertTailList(&FreeList, &entry->Link);	

	++FreeCount;
	_ASSERT(FreeCount <= Quota);
}

template<typename T>
T* IResourcePool<T>::get()
{
	if (IsListEmpty(&FreeList))
		return NULL;

	LENTRY* p = RemoveHeadList(&FreeList);
	SEntry* entry = reinterpret_cast<SEntry*>CONTAINING_RECORD(p, SEntry, Link);
	--FreeCount;

	return &entry->Data;
}

template<typename T>
void IResourcePool<T>::allocateAll( u32 quota, bool temp )
{
	if(Entries)
		return;

	Quota = quota;
	Temp = temp;

	if (Temp)
		Entries = (SEntry*)Hunk_AllocateTempMemory(sizeof(SEntry)*Quota);
	else
		Entries = new SEntry[Quota];

	for (u32 i=0; i<Quota; ++i)
	{
		InsertTailList(&FreeList, &Entries[i].Link);
	}

	FreeCount = Quota;
}