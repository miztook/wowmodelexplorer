#include "stdafx.h"
#include "base.h"
#include "q_memory.h"

typedef unsigned char 		byte;

/*
==============================================================================

						ZONE MEMORY ALLOCATION

There is never any space between memblocks, and there will never be two
contiguous free memblocks.

The rover can be left pointing at a non-empty block

The zone calls are pretty much only used for small strings and structures,
all big things are allocated on the hunk.
==============================================================================
*/

#define	ZONEID	0x1d4a11
#define MINFRAGMENT	64

typedef struct zonedebug_s {
	char *label;
	char *file;
	int line;
	int allocSize;
} zonedebug_t;

typedef struct memblock_s {
	int	size;           // including the header and possibly tiny fragments
	int     tag;            // a tag of 0 is a free block
	struct memblock_s       *next, *prev;
	int     id;        		// should be ZONEID
#ifdef ZONE_DEBUG
	zonedebug_t d;
#endif
} memblock_t;

typedef struct {
	int		size;			// total bytes malloced, including header
	int		used;			// total bytes used
	memblock_t	blocklist;	// start / end cap for linked list
	memblock_t	*rover;
} memzone_t;

// main zone for all "dynamic" memory allocation
memzone_t*		mainzone;
// we also have a small zone for small allocations that would only
// fragment the main zone (think of cvar and cmd strings)
memzone_t*		smallzone;

// temp zone for temporary memory allocation
memzone_t*		tempzone;

void Z_CheckHeap( void );

/*
========================
Z_CheckHeap
========================
*/
void Z_CheckHeap( void ) {
	memblock_t	*block;
	
	for (block = mainzone->blocklist.next ; ; block = block->next) {
		if (block->next == &mainzone->blocklist) {
			break;			// all blocks have been hit
		}
		if ( (byte *)block + block->size != (byte *)block->next) {
			//Com_Error( ERR_FATAL, "Z_CheckHeap: block size does not touch the next block\n" );
			ASSERT(false);
		}
		if ( block->next->prev != block) {
			//Com_Error( ERR_FATAL, "Z_CheckHeap: next block doesn't have proper back link\n" );
			ASSERT(false);
		}
		if ( !block->tag && !block->next->tag ) {
			//Com_Error( ERR_FATAL, "Z_CheckHeap: two consecutive free blocks\n" );
			ASSERT(false);
		}
	}
}

static	size_t		s_zoneTotal;
static	size_t		s_smallZoneTotal;
static	size_t		s_tempZoneTotal;

/*
=================
QMem_Info
=================
*/
void QMem_Info( void ) {
	memblock_t	*block;
	int			zoneBytes, zoneBlocks;
	int			smallZoneBytes, smallZoneBlocks;

	zoneBytes = 0;
	zoneBlocks = 0;
	for (block = mainzone->blocklist.next ; ; block = block->next) {
	
		if ( block->tag ) {
			zoneBytes += block->size;
			zoneBlocks++;
		}

		if (block->next == &mainzone->blocklist) {
			break;			// all blocks have been hit	
		}
		if ( (byte *)block + block->size != (byte *)block->next) {
			printf ("ERROR: block size does not touch the next block\n");
		}
		if ( block->next->prev != block) {
			printf ("ERROR: next block doesn't have proper back link\n");
		}
		if ( !block->tag && !block->next->tag ) {
			printf ("ERROR: two consecutive free blocks\n");
		}
	}

	smallZoneBytes = 0;
	smallZoneBlocks = 0;
	for (block = smallzone->blocklist.next ; ; block = block->next) {
		if ( block->tag ) {
			smallZoneBytes += block->size;
			smallZoneBlocks++;
		}

		if (block->next == &smallzone->blocklist) {
			break;			// all blocks have been hit	
		}
	}

	printf( "%8i bytes in %i zone blocks\n", zoneBytes, zoneBlocks	);
	printf( "        %8i bytes in dynamic other\n", zoneBytes );
	printf( "        %8i bytes in small Zone memory\n", smallZoneBytes );
}

/*
========================
Z_ClearZone
========================
*/
void Z_ClearZone( memzone_t *zone, size_t size ) {
	memblock_t	*block;
	
	// set the entire zone to one free block

	zone->blocklist.next = zone->blocklist.prev = block =
		(memblock_t *)( (byte *)zone + sizeof(memzone_t) );
	zone->blocklist.tag = 1;	// in use block
	zone->blocklist.id = 0;
	zone->blocklist.size = 0;
	zone->rover = block;
	zone->size = (int)size;
	zone->used = 0;
	
	block->prev = block->next = &zone->blocklist;
	block->tag = 0;			// free block
	block->id = ZONEID;
	block->size = (int)(size - sizeof(memzone_t));
}

/*
=================
Com_InitZoneMemory
=================
*/
void Com_InitSmallZoneMemory( size_t cv ) {
	if (cv < 1)
		cv = 1;

	s_smallZoneTotal = 1024 * 1024 * cv;

	// bk001205 - was malloc
	smallzone = (memzone_t *)calloc( s_smallZoneTotal, 1 );
	if ( !smallzone ) {
		/*Com_Error( ERR_FATAL, "Small zone data failed to allocate %1.1f megs", (float)s_smallZoneTotal / (1024*1024) );*/
		ASSERT(false);
	}
	Z_ClearZone( smallzone, s_smallZoneTotal );
	
	return;
}

void Com_InitMainZoneMemory( size_t cv ) {
	if (cv < 1)
		cv = 1;
	// allocate the random block zone
	s_zoneTotal = cv * 1024 * 1024;

	// bk001205 - was malloc
	mainzone = (memzone_t *)calloc( s_zoneTotal, 1 );
	if ( !mainzone ) {
		/*Com_Error( ERR_FATAL, "Zone data failed to allocate %i megs", s_zoneTotal / (1024*1024) );*/
		ASSERT(false);
	}
	Z_ClearZone( mainzone, s_zoneTotal );
}

void Com_InitTempZoneMemory( size_t cv ){
	if (cv < 1)
		cv = 1;

	// allocate the random block zone
	s_tempZoneTotal = cv * 1024 * 1024;

	// bk001205 - was malloc
	tempzone = (memzone_t *)calloc( s_tempZoneTotal, 1 );
	if ( !tempzone ) {
		/*Com_Error( ERR_FATAL, "Zone data failed to allocate %i megs", s_zoneTotal / (1024*1024) );*/
		ASSERT(false);
	}
	Z_ClearZone( tempzone, s_tempZoneTotal );
}

/*
=================
Com_ReleaseZoneMemory
=================
*/
void Com_ReleaseSmallZoneMemory( void )
{
	free(smallzone);
}
 
void Com_ReleaseMainZoneMemory( void )
{
	free(mainzone);
}

void Com_ReleaseTempZoneMemory( void )
{
	free(tempzone);
}

/*
================
Z_TagMalloc
================
*/
#ifdef ZONE_DEBUG
void *Z_TagMallocDebug( size_t size, int tag, char *label, char *file, int line ) {
#else
void *Z_TagMalloc( size_t size, int tag ) {
#endif
	size_t		extra, allocSize;
	memblock_t	*start, *rover, *newblock, *base;
	memzone_t *zone;

	if (!tag) {
		ASSERT(false);//Com_Error( ERR_FATAL, "Z_TagMalloc: tried to use a 0 tag" );
	}

	if ( tag == TAG_SMALL ) {
		zone = smallzone;
	}
	else if ( tag == TAG_TEMP ){
		zone = tempzone;
	}
	else {
		zone = mainzone;
	}

	allocSize = size;

	//
	// scan through the block list looking for the first free block
	// of sufficient size
	//
	size += sizeof(memblock_t);	// account for size of block header
	size += 4;					// space for memory trash tester
	size = (size + 3) & ~3;		// align to 32 bit boundary
	
	base = rover = zone->rover;
	start = base->prev;
	
	do {
		if (rover == start)	{
			// scaned all the way around the list
// 			Com_Error( ERR_FATAL, "Z_Malloc: failed on allocation of %i bytes from the %s zone",
// 								size, zone == smallzone ? "small" : "main");
//			ASSERT(false);
			return NULL_PTR;
		}
		if (rover->tag) {
			base = rover = rover->next;
		} else {
			rover = rover->next;
		}
	} while (base->tag || base->size < (int)size);
	
	//
	// found a block big enough
	//
	extra = base->size - size;
	if (extra > MINFRAGMENT) {
		// there will be a free fragment after the allocated block
		newblock = (memblock_t *) ((byte *)base + size );
		newblock->size = (int)extra;
		newblock->tag = 0;			// free block
		newblock->prev = base;
		newblock->id = ZONEID;
		newblock->next = base->next;
		newblock->next->prev = newblock;
		base->next = newblock;
		base->size = (int)size;
	}
	
	base->tag = tag;			// no longer a free block
	
	zone->rover = base->next;	// next allocation will start looking here
	zone->used += base->size;	//
	
	base->id = ZONEID;

#ifdef ZONE_DEBUG
	base->d.label = label;
	base->d.file = file;
	base->d.line = line;
	base->d.allocSize = (u32)allocSize;
#endif

	// marker for memory trash testing
	*(int *)((byte *)base + base->size - 4) = ZONEID;

	return (void *) ((byte *)base + sizeof(memblock_t));
}

/*
========================
Z_Free
========================
*/
bool Z_Free( void *ptr ) {
	memblock_t	*block, *other;
	memzone_t *zone;
	
	if (!ptr) {
		//Com_Error( ERR_DROP, "Z_Free: NULL_PTR pointer" );
		ASSERT(false);
		return true;
	}

	block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	if (block->id != ZONEID) {
		//Com_Error( ERR_FATAL, "Z_Free: freed a pointer without ZONEID" );
		//ASSERT(false);
		return false;
	}
	if (block->tag == 0) {
		//Com_Error( ERR_FATAL, "Z_Free: freed a freed pointer" );
		ASSERT(false);
	}

	// check the memory trash tester
	if (  *(int *)((byte *)block + block->size - 4 ) != ZONEID ) {
		//Com_Error( ERR_FATAL, "Z_Free: memory block wrote past end" );
		ASSERT(false);
	}

	if (block->tag == TAG_SMALL) {
		zone = smallzone;
	} 
	else if (block->tag == TAG_TEMP){
		zone = tempzone;
	}
	else {
		zone = mainzone;
	}

	zone->used -= block->size;
	// set the block to something that should cause problems
	// if it is referenced...
	memset( ptr, 0xcc, block->size - sizeof( *block ) );

	block->tag = 0;		// mark as free
	
	other = block->prev;
	if (!other->tag) {
		// merge with previous free block
		other->size += block->size;
		other->next = block->next;
		other->next->prev = other;
		if (block == zone->rover) {
			zone->rover = other;
		}
		block = other;
	}

	zone->rover = block;

	other = block->next;
	if ( !other->tag ) {
		// merge the next free block onto the end
		block->size += other->size;
		block->next = other->next;
		block->next->prev = block;
		if (other == zone->rover) {
			zone->rover = block;
		}
	}

	return true;
}

/*
========================
Z_Realloc
========================
*/
void* Z_Realloc( void* ptr, size_t size, int tag, bool& success )
{
	success = true;

	if (!size && ptr)
	{
		Z_Free(ptr);
		return NULL_PTR;
	}

	if (!ptr)
		return Z_TagMalloc(size, tag);

	memblock_t* base = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	if (base->tag == 0) {
		ASSERT(false);
		success = false;
		return NULL_PTR;
	}

	// check the memory trash tester
	if (  *(int *)((byte *)base + base->size - 4 ) != ZONEID ) {
		ASSERT(false);
		success = false;
		return NULL_PTR;
	}

	int newsize = (int)size;
	newsize += sizeof(memblock_t);	// account for size of block header
	newsize += 4;					// space for memory trash tester
	newsize = (newsize + 3) & ~3;

	if (newsize < base->size)			//big enough, return memory block unchanged
	{
		int extra = base->size - newsize;
		if (extra > MINFRAGMENT) {
			// there will be a free fragment after the allocated block
			memblock_t* newblock = (memblock_t *) ((byte *)base + newsize );
			newblock->size = extra;
			newblock->tag = 0;			// free block
			newblock->prev = base;
			newblock->id = ZONEID;
			newblock->next = base->next;
			newblock->next->prev = newblock;
			base->next = newblock;
			base->size = newsize;

			*(int *)((byte *)base + base->size - 4) = ZONEID;
		}
		return ptr;
	}

	//alloc new memory
	void* newptr = Z_TagMalloc(size, tag);
	if (!newptr)
		return NULL_PTR;

	if (newptr != ptr)
	{
		memcpy(newptr, ptr, size);
		Z_Free(ptr);
	}

	return newptr;
}

/*
========================
Z_AvailableZoneMemory
========================
*/
size_t Z_AvailableZoneMemory( memzone_t *zone ) {
	return zone->size - zone->used;
}

/*
========================
Z_AvailableMemory
========================
*/
size_t Z_AvailableMainMemory( void ) {
	return Z_AvailableZoneMemory( mainzone );
}

size_t Z_AvailableSmallMemory( void ){
	return Z_AvailableZoneMemory( smallzone );
}

size_t Z_AvailableTempMemory( void ){
	return Z_AvailableZoneMemory( tempzone );
}

float Z_AvailableMainMemoryPercent(){
	return (mainzone->size - mainzone->used) / (float)mainzone->size;
}
float Z_AvailableSmallMemoryPercent(){
	return (smallzone->size - smallzone->used) / (float)smallzone->size;
}
float Z_AvailableTempMemoryPercent(){
	return (tempzone->size - tempzone->used) / (float)tempzone->size;
}


/*
================
T_TagMalloc
================
*/
#ifdef TEMP_DEBUG
void *T_TagMallocDebug( size_t size, int tag, char *label, char *file, int line ) {
#else
void *T_TagMalloc( size_t size, int tag ) {
#endif
    
	size_t		extra, allocSize;
	memblock_t	*start, *rover, *newblock, *base;
	memzone_t *zone = tempzone;

	if (!tag) {
		ASSERT(false);//Com_Error( ERR_FATAL, "Z_TagMalloc: tried to use a 0 tag" );
	}

	allocSize = size;
	//
	// scan through the block list looking for the first free block
	// of sufficient size
	//
	size += sizeof(memblock_t);	// account for size of block header
	size += 4;					// space for memory trash tester
	size = (size + 3) & ~3;		// align to 32 bit boundary
	
	base = rover = zone->rover;
	start = base->prev;
	
	do {
		if (rover == start)	{
			// scaned all the way around the list
// 			Com_Error( ERR_FATAL, "Z_Malloc: failed on allocation of %i bytes from the %s zone",
// 								size, zone == smallzone ? "small" : "main");
			ASSERT(false);
			return NULL_PTR;
		}
		if (rover->tag) {
			base = rover = rover->next;
		} else {
			rover = rover->next;
		}
	} while (base->tag || base->size < (int)size);
	
	//
	// found a block big enough
	//
	extra = base->size - size;
	if (extra > MINFRAGMENT) {
		// there will be a free fragment after the allocated block
		newblock = (memblock_t *) ((byte *)base + size );
		newblock->size = (int)extra;
		newblock->tag = 0;			// free block
		newblock->prev = base;
		newblock->id = ZONEID;
		newblock->next = base->next;
		newblock->next->prev = newblock;
		base->next = newblock;
		base->size = (int)size;
	}
	
	base->tag = tag;			// no longer a free block
	
	zone->rover = base->next;	// next allocation will start looking here
	zone->used += base->size;	//
	
	base->id = ZONEID;

#ifdef ZONE_DEBUG
	base->d.label = label;
	base->d.file = file;
	base->d.line = line;
	base->d.allocSize = (int)allocSize;
#endif

	// marker for memory trash testing
	*(int *)((byte *)base + base->size - 4) = ZONEID;

	return (void *) ((byte *)base + sizeof(memblock_t));
}

/*
========================
T_Free
========================
*/
bool T_Free( void* ptr ) {
	memblock_t	*block, *other;
	memzone_t *zone = tempzone;
	
	if (!ptr) {
		//Com_Error( ERR_DROP, "Z_Free: NULL_PTR pointer" );
		ASSERT(false);
		return true;
	}

	block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	if (block->id != ZONEID) {
		//Com_Error( ERR_FATAL, "Z_Free: freed a pointer without ZONEID" );
		//ASSERT(false);
		return false;
	}
	if (block->tag == 0) {
		//Com_Error( ERR_FATAL, "Z_Free: freed a freed pointer" );
		ASSERT(false);
	}

	// check the memory trash tester
	if (  *(int *)((byte *)block + block->size - 4 ) != ZONEID ) {
		//Com_Error( ERR_FATAL, "Z_Free: memory block wrote past end" );
		ASSERT(false);
	}

	zone->used -= block->size;
	// set the block to something that should cause problems
	// if it is referenced...
	memset( ptr, 0xcc, block->size - sizeof( *block ) );

	block->tag = 0;		// mark as free
	
	other = block->prev;
	if (!other->tag) {
		// merge with previous free block
		other->size += block->size;
		other->next = block->next;
		other->next->prev = other;
		if (block == zone->rover) {
			zone->rover = other;
		}
		block = other;
	}

	zone->rover = block;

	other = block->next;
	if ( !other->tag ) {
		// merge the next free block onto the end
		block->size += other->size;
		block->next = other->next;
		block->next->prev = block;
		if (other == zone->rover) {
			zone->rover = block;
		}
	}

	return true;
}

/*
========================
T_Realloc
========================
*/
void* T_Realloc( void* ptr, size_t size, int tag, bool& success )
{
	success = true;

	if (!size && ptr)
	{
		T_Free(ptr);
		return NULL_PTR;
	}

	if (!ptr)
		return T_TagMalloc(size, tag);

	memblock_t* base = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	if (base->tag == 0) {
		ASSERT(false);
		success = false;
		return NULL_PTR;
	}

	// check the memory trash tester
	if (  *(int *)((byte *)base + base->size - 4 ) != ZONEID ) {
		ASSERT(false);
		success = false;
		return NULL_PTR;
	}

	size_t newsize = size;
	newsize += sizeof(memblock_t);	// account for size of block header
	newsize += 4;					// space for memory trash tester
	newsize = (newsize + 3) & ~3;

	if ((int)newsize < base->size)			//big enough, return memory block unchanged
	{
		int extra = base->size - (int)newsize;
		if (extra > MINFRAGMENT) {
			// there will be a free fragment after the allocated block
			memblock_t* newblock = (memblock_t *) ((byte *)base + newsize );
			newblock->size = extra;
			newblock->tag = 0;			// free block
			newblock->prev = base;
			newblock->id = ZONEID;
			newblock->next = base->next;
			newblock->next->prev = newblock;
			base->next = newblock;
			base->size = (int)newsize;

			*(int *)((byte *)base + base->size - 4) = ZONEID;
		}
		return ptr;
	}

	//alloc new memory
	void* newptr = T_TagMalloc(size, tag);
	if (!newptr)
		return NULL_PTR;

	if (newptr != ptr)
	{
		memcpy(newptr, ptr, size);
		T_Free(ptr);
	}

	return newptr;
}


void QMem_Init(size_t smallZoneM, size_t generalZoneM, size_t tempZoneM)
{
	Com_InitSmallZoneMemory(smallZoneM);		//
	Com_InitMainZoneMemory(generalZoneM);			//
	Com_InitTempZoneMemory(tempZoneM);
}

void QMem_End()
{
	Com_ReleaseTempZoneMemory();
	Com_ReleaseMainZoneMemory();
	Com_ReleaseSmallZoneMemory();
}
