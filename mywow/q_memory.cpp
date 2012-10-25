#include "stdafx.h"
#include "base.h"
#include "q_memory.h"

extern u32 g_MainThreadId;

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
	int		size;           // including the header and possibly tiny fragments
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
memzone_t*		renderzone;		//for rendering use, no lock

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
			_ASSERT(false);
		}
		if ( block->next->prev != block) {
			//Com_Error( ERR_FATAL, "Z_CheckHeap: next block doesn't have proper back link\n" );
			_ASSERT(false);
		}
		if ( !block->tag && !block->next->tag ) {
			//Com_Error( ERR_FATAL, "Z_CheckHeap: two consecutive free blocks\n" );
			_ASSERT(false);
		}
	}
}


/*
==============================================================================

Goals:
	reproducable without history effects -- no out of memory errors on weird map to map changes
	allow restarting of the client without fragmentation
	minimize total pages in use at run time
	minimize total pages needed during load time

  Single block of memory with stack allocators coming from both ends towards the middle.

  One side is designated the temporary memory allocator.

  Temporary memory can be allocated and freed in any order.

  A highwater mark is kept of the most in use at any time.

  When there is no temporary memory allocated, the permanent and temp sides
  can be switched, allowing the already touched temp memory to be used for
  permanent storage.

  Temp memory must never be allocated on two ends at once, or fragmentation
  could occur.

  If we have any in-use temp memory, additional temp allocations must come from
  that side.

  If not, we can choose to make either side the new temp side and push future
  permanent allocations to the other side.  Permanent allocations should be
  kept on the side that has the current greatest wasted highwater mark.

==============================================================================
*/


#define	HUNK_MAGIC	0x89537892
#define	HUNK_FREE_MAGIC	0x89537893

typedef struct {
	int		magic;
	int		size;
} hunkHeader_t;

typedef struct {
	int		mark;
	int		temp;
	int		tempHighwater;
} hunkUsed_t;

typedef struct hunkblock_s {
	int size;
	byte printed;
	struct hunkblock_s *next;
	char *label;
	char *file;
	int line;
} hunkblock_t;

//main thread
static	hunkUsed_t	hunk_high;
static	hunkUsed_t* hunk_temp;

//other thread
static hunkUsed_t		hunk_high_other;
static hunkUsed_t*	hunk_temp_other;

static	byte*	s_hunkData = NULL;
static	byte*	s_hunkMem = NULL;
static	int		s_hunkTotal;

static	byte*	s_hunkData_other = NULL;
static	byte*	s_hunkMem_other = NULL;
static	int		s_hunkTotal_other;

static	int		s_zoneTotal;
static	int		s_smallZoneTotal;
static	int		s_renderZoneTotal;

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

	printf( "%8i bytes total hunk\n", s_hunkTotal );
	printf( "%8i bytes total zone\n", s_zoneTotal );
	printf( "\n" );
	printf( "%8i high mark\n", hunk_high.mark );
	printf( "%8i high tempHighwater\n", hunk_high.tempHighwater );
	printf( "\n" );
	printf( "%8i bytes in %i zone blocks\n", zoneBytes, zoneBlocks	);
	printf( "        %8i bytes in dynamic other\n", zoneBytes );
	printf( "        %8i bytes in small Zone memory\n", smallZoneBytes );
}

/*
========================
Z_ClearZone
========================
*/
void Z_ClearZone( memzone_t *zone, int size ) {
	memblock_t	*block;
	
	// set the entire zone to one free block

	zone->blocklist.next = zone->blocklist.prev = block =
		(memblock_t *)( (byte *)zone + sizeof(memzone_t) );
	zone->blocklist.tag = 1;	// in use block
	zone->blocklist.id = 0;
	zone->blocklist.size = 0;
	zone->rover = block;
	zone->size = size;
	zone->used = 0;
	
	block->prev = block->next = &zone->blocklist;
	block->tag = 0;			// free block
	block->id = ZONEID;
	block->size = size - sizeof(memzone_t);
}

/*
=================
Com_InitZoneMemory
=================
*/
void Com_InitSmallZoneMemory( int cv ) {
	if ( cv < 4 )
		s_smallZoneTotal = 1024 * 1024 * 4;
	else
		s_smallZoneTotal = 1024 * 1024 * cv;
	// bk001205 - was malloc
	smallzone = (memzone_t *)calloc( s_smallZoneTotal, 1 );
	if ( !smallzone ) {
		/*Com_Error( ERR_FATAL, "Small zone data failed to allocate %1.1f megs", (float)s_smallZoneTotal / (1024*1024) );*/
		_ASSERT(false);
	}
	Z_ClearZone( smallzone, s_smallZoneTotal );
	
	return;
}

void Com_InitMainZoneMemory( int cv ) {

	// allocate the random block zone
 	if ( cv < 16 ) {
 		s_zoneTotal = 1024 * 1024 * 16;
 	} else {
		s_zoneTotal = cv * 1024 * 1024;
	}

	// bk001205 - was malloc
	mainzone = (memzone_t *)calloc( s_zoneTotal, 1 );
	if ( !mainzone ) {
		/*Com_Error( ERR_FATAL, "Zone data failed to allocate %i megs", s_zoneTotal / (1024*1024) );*/
		_ASSERT(false);
	}
	Z_ClearZone( mainzone, s_zoneTotal );
}

void Com_InitRenderZoneMemory( int cv )
{
		s_renderZoneTotal = 1024 * 1024 * cv;

		renderzone = (memzone_t*)calloc( s_renderZoneTotal, 1);
		if ( !renderzone )
		{
			_ASSERT(false);
		}
		Z_ClearZone( renderzone, s_renderZoneTotal );
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

void Com_ReleaseRenderZoneMemory(void)
{
	free(renderzone);
}

/*
================
Z_TagMalloc
================
*/
#ifdef ZONE_DEBUG
void *Z_TagMallocDebug( int size, int tag, char *label, char *file, int line ) {
#else
void *Z_TagMalloc( int size, int tag ) {
#endif
	int		extra, allocSize;
	memblock_t	*start, *rover, *newblock, *base;
	memzone_t *zone;

	if (!tag) {
		_ASSERT(false);//Com_Error( ERR_FATAL, "Z_TagMalloc: tried to use a 0 tag" );
	}

	if ( tag == TAG_SMALL ) {
		zone = smallzone;
	}
	else if( tag == TAG_RENDERER ) {
		zone = renderzone;
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
			_ASSERT(false);
			return NULL;
		}
		if (rover->tag) {
			base = rover = rover->next;
		} else {
			rover = rover->next;
		}
	} while (base->tag || base->size < size);
	
	//
	// found a block big enough
	//
	extra = base->size - size;
	if (extra > MINFRAGMENT) {
		// there will be a free fragment after the allocated block
		newblock = (memblock_t *) ((byte *)base + size );
		newblock->size = extra;
		newblock->tag = 0;			// free block
		newblock->prev = base;
		newblock->id = ZONEID;
		newblock->next = base->next;
		newblock->next->prev = newblock;
		base->next = newblock;
		base->size = size;
	}
	
	base->tag = tag;			// no longer a free block
	
	zone->rover = base->next;	// next allocation will start looking here
	zone->used += base->size;	//
	
	base->id = ZONEID;

#ifdef ZONE_DEBUG
	base->d.label = label;
	base->d.file = file;
	base->d.line = line;
	base->d.allocSize = allocSize;
#endif

	// marker for memory trash testing
	*(int *)((byte *)base + base->size - 4) = ZONEID;

	return (void *) ((byte *)base + sizeof(memblock_t));
}

/*
========================
Z_Malloc
========================
*/
#ifdef ZONE_DEBUG
void *Z_MallocDebug( int size, char *label, char *file, int line ) {
#else
void *Z_Malloc( int size ) {
#endif
	void	*buf;
	
  //Z_CheckHeap ();	// DEBUG
	
#ifdef ZONE_DEBUG
	buf = Z_TagMallocDebug( size, TAG_GENERAL, label, file, line );
#else
	buf = Z_TagMalloc( size, TAG_GENERAL );
#endif

	memset( buf, 0, size );

	return buf;
}

#ifdef ZONE_DEBUG
void *S_MallocDebug( int size, char *label, char *file, int line ) {
	return Z_TagMallocDebug( size, TAG_SMALL, label, file, line );
}
#else
void *S_Malloc( int size ) {
	return Z_TagMalloc( size, TAG_SMALL );
}
#endif

/*
========================
Z_Free
========================
*/
bool Z_Free( void *ptr ) {
	memblock_t	*block, *other;
	memzone_t *zone;
	
	if (!ptr) {
		//Com_Error( ERR_DROP, "Z_Free: NULL pointer" );
		_ASSERT(false);
		return true;
	}

	block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
	if (block->id != ZONEID) {
		//Com_Error( ERR_FATAL, "Z_Free: freed a pointer without ZONEID" );
		_ASSERT(false);
		return false;
	}
	if (block->tag == 0) {
		//Com_Error( ERR_FATAL, "Z_Free: freed a freed pointer" );
		_ASSERT(false);
	}

	// check the memory trash tester
	if (  *(int *)((byte *)block + block->size - 4 ) != ZONEID ) {
		//Com_Error( ERR_FATAL, "Z_Free: memory block wrote past end" );
		_ASSERT(false);
	}

	if (block->tag == TAG_SMALL) {
		zone = smallzone;
	} else if (block->tag == TAG_RENDERER) {
		zone = renderzone;
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
Z_AvailableZoneMemory
========================
*/
int Z_AvailableZoneMemory( memzone_t *zone ) {
	return zone->size - zone->used;
}

/*
========================
Z_AvailableMemory
========================
*/
int Z_AvailableMainMemory( void ) {
	return Z_AvailableZoneMemory( mainzone );
}

int Z_AvailableSmallMemory( void ){
	return Z_AvailableZoneMemory( smallzone );
}

int Z_AvailableRenderMemory( void ) {
	return Z_AvailableZoneMemory( renderzone );
}

float Z_AvailableMainMemoryPercent(){
	return (mainzone->size - mainzone->used) / (float)mainzone->size;
}
float Z_AvailableSmallMemoryPercent(){
	return (smallzone->size - smallzone->used) / (float)smallzone->size;
}
float Z_AvailableRenderMemoryPercent(){
	return (renderzone->size - renderzone->used) / (float)renderzone->size;
}

/*
=================
Com_InitHunkMemory
=================
*/
void Com_InitHunkMemory( int cv, int cv_other) {
	s_hunkTotal = 1024 * 1024 * cv;
	s_hunkTotal_other = 1024 * 1024 * cv_other;

	// bk001205 - was malloc
	s_hunkMem = (byte*)calloc( s_hunkTotal + 31, 1 );
	if ( !s_hunkMem ) {
		_ASSERT(false);
	}
	// cacheline align
	s_hunkData = (byte *) ( ( (int)s_hunkMem + 31 ) & ~31 );

	s_hunkMem_other = (byte*)calloc( s_hunkTotal_other + 31, 1 );
	if ( !s_hunkMem_other ) {
		_ASSERT(false);
	}
	// cacheline align
	s_hunkData_other = (byte *) ( ( (int)s_hunkMem_other + 31 ) & ~31 );

	Hunk_Clear();
}

/*
=================
Com_ReleaseHunkMemory
=================
*/
void Com_ReleaseHunkMemory()
{
	free(s_hunkMem_other);
	free(s_hunkMem);
}


/*
=================
Hunk_Clear

=================
*/
void Hunk_Clear( void ) {
	hunk_high.mark = 0;
	hunk_high.temp = 0;
	hunk_high.tempHighwater = 0;

	hunk_temp = &hunk_high;

	hunk_high_other.mark = 0;
	hunk_high_other.temp = 0;
	hunk_high_other.tempHighwater = 0;

	hunk_temp_other = &hunk_high_other;
}

/*
=================
Hunk_AllocateTempMemory

This is used by the file loading system.
Multiple files can be loaded in temporary memory.
When the files-in-use count reaches zero, all temp memory will be deleted
=================
*/
void *Hunk_AllocateTempMemory( int size ) {
	if( g_MainThreadId == ::GetCurrentThreadId() )
		return Hunk_AllocateTempMemory_Main(size);
	else
		return Hunk_AllocateTempMemory_Other(size);
}

/*
==================
Hunk_FreeTempMemory
==================
*/
void Hunk_FreeTempMemory( void *buf ) {
	if( g_MainThreadId == ::GetCurrentThreadId() )
		Hunk_FreeTempMemory_Main(buf);
	else
		Hunk_FreeTempMemory_Other(buf);	
}

/*
====================
Hunk_MemoryRemaining
====================
*/
int	Hunk_MemoryRemaining( void ) {
	if( g_MainThreadId == ::GetCurrentThreadId() )
		return Hunk_MemoryRemaining_Main();
	else
		return Hunk_MemoryRemaining_Other();
}

void QMem_Init(size_t smallZoneM, size_t generalZoneM, size_t renderZoneM, size_t hunkSizeM, size_t hunkSizeOtherM)
{
	Com_InitSmallZoneMemory(smallZoneM);		//
	Com_InitMainZoneMemory(generalZoneM);			//
	Com_InitRenderZoneMemory(renderZoneM);
	Com_InitHunkMemory(hunkSizeM, hunkSizeOtherM);			//
}

void QMem_End()
{
	Com_ReleaseHunkMemory();
	Com_ReleaseRenderZoneMemory();
	Com_ReleaseMainZoneMemory();
	Com_ReleaseSmallZoneMemory();
}

void* Hunk_AllocateTempMemory_Main( int size )
{
	void		*buf;
	hunkHeader_t	*hdr;

	// return a Z_Malloc'd block if the hunk has not been initialized
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different 
	// memory systems
	if ( s_hunkData == NULL )
	{
		void* ret = Z_Malloc(size);
		return ret;
	}

	size = ( (size+3)&~3 ) + sizeof( hunkHeader_t );

	if ( hunk_temp->temp + size > s_hunkTotal ) {
		//Com_Error( ERR_DROP, "Hunk_AllocateTempMemory: failed on %i", size );
		_ASSERT(false);
	}

	hunk_temp->temp += size;
	buf = (void *)(s_hunkData + s_hunkTotal - hunk_temp->temp );

	if ( hunk_temp->temp > hunk_temp->tempHighwater ) {
		hunk_temp->tempHighwater = hunk_temp->temp;
	}

	hdr = (hunkHeader_t *)buf;
	buf = (void *)(hdr+1);

	hdr->magic = HUNK_MAGIC;
	hdr->size = size;

	// don't bother clearing, because we are going to load a file over it
	return buf;
}

void Hunk_FreeTempMemory_Main( void* buf )
{
	hunkHeader_t	*hdr;

	// free with Z_Free if the hunk has not been initialized
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different 
	// memory systems
	if ( s_hunkData == NULL )
	{
		Z_Free(buf);
		return;
	}


	hdr = ( (hunkHeader_t *)buf ) - 1;
	if ( hdr->magic != HUNK_MAGIC ) {
		//Com_Error( ERR_FATAL, "Hunk_FreeTempMemory: bad magic" );
		_ASSERT(false);
	}

	hdr->magic = HUNK_FREE_MAGIC;

	// this only works if the files are freed in stack order,
	// otherwise the memory will stay around until Hunk_ClearTempMemory
	if ( hdr == (void *)(s_hunkData + s_hunkTotal - hunk_temp->temp ) ) {
		hunk_temp->temp -= hdr->size;
	} else {
		//Com_Printf( "Hunk_FreeTempMemory: not the final block\n" );
		_ASSERT(false);
	}
}

int Hunk_MemoryRemaining_Main( void )
{
	int		low, high;

	low = 0;
	high = hunk_high.temp;

	return s_hunkTotal - ( low + high );
}

void* Hunk_AllocateTempMemory_Other( int size )
{
	void		*buf;
	hunkHeader_t	*hdr;

	// return a Z_Malloc'd block if the hunk has not been initialized
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different 
	// memory systems
	if ( s_hunkData_other == NULL )
	{
		void* ret = Z_Malloc(size);
		return ret;
	}

	size = ( (size+3)&~3 ) + sizeof( hunkHeader_t );

	if ( hunk_temp_other->temp + size > s_hunkTotal_other ) {
		//Com_Error( ERR_DROP, "Hunk_AllocateTempMemory: failed on %i", size );
		_ASSERT(false);
	}

	hunk_temp_other->temp += size;
	buf = (void *)(s_hunkData_other + s_hunkTotal_other - hunk_temp_other->temp );

	if ( hunk_temp_other->temp > hunk_temp_other->tempHighwater ) {
		hunk_temp_other->tempHighwater = hunk_temp_other->temp;
	}

	hdr = (hunkHeader_t *)buf;
	buf = (void *)(hdr+1);

	hdr->magic = HUNK_MAGIC;
	hdr->size = size;

	// don't bother clearing, because we are going to load a file over it
	return buf;
}

void Hunk_FreeTempMemory_Other( void* buf )
{
	hunkHeader_t	*hdr;

	// free with Z_Free if the hunk has not been initialized
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different 
	// memory systems
	if ( s_hunkData_other == NULL )
	{
		Z_Free(buf);
		return;
	}


	hdr = ( (hunkHeader_t *)buf ) - 1;
	if ( hdr->magic != HUNK_MAGIC ) {
		//Com_Error( ERR_FATAL, "Hunk_FreeTempMemory: bad magic" );
		_ASSERT(false);
	}

	hdr->magic = HUNK_FREE_MAGIC;

	// this only works if the files are freed in stack order,
	// otherwise the memory will stay around until Hunk_ClearTempMemory
	if ( hdr == (void *)(s_hunkData_other + s_hunkTotal_other - hunk_temp_other->temp ) ) {
		hunk_temp_other->temp -= hdr->size;
	} else {
		//Com_Printf( "Hunk_FreeTempMemory: not the final block\n" );
		_ASSERT(false);
	}
}

int Hunk_MemoryRemaining_Other( void )
{
	int		low, high;

	low = 0;
	high = hunk_high_other.temp;

	return s_hunkTotal_other - ( low + high );
}

