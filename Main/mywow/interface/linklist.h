#pragma once

typedef struct _LENTRY {
	struct _LENTRY *Flink;
	struct _LENTRY *Blink;
} LENTRY, *PLENTRY;


inline void InitializeListHead(PLENTRY ListHead)
{
	ListHead->Flink = ListHead->Blink = ListHead;
}

#define IsListEmpty(ListHead) \
	((ListHead)->Flink == (ListHead))

inline bool RemoveEntryList(PLENTRY Entry)
{
	PLENTRY Blink;
	PLENTRY Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;

	return (Flink == Blink);
}

inline PLENTRY RemoveHeadList(PLENTRY ListHead)
{
	PLENTRY Flink;
	PLENTRY Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;
	return Entry;
}


inline PLENTRY RemoveTailList(PLENTRY ListHead)
{
	PLENTRY Blink;
	PLENTRY Entry;

	Entry = ListHead->Blink;
	Blink = Entry->Blink;
	ListHead->Blink = Blink;
	Blink->Flink = ListHead;
	return Entry;
}


inline void InsertTailList(PLENTRY ListHead, PLENTRY Entry)
{
	PLENTRY Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}

inline void InsertHeadList( PLENTRY ListHead, PLENTRY Entry)
{
	PLENTRY Flink;

	Flink = ListHead->Flink;
	Entry->Flink = Flink;
	Entry->Blink = ListHead;
	Flink->Blink = Entry;
	ListHead->Flink = Entry;
}