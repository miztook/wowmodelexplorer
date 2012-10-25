#pragma once

typedef enum {
	TAG_FREE = 0,
	TAG_GENERAL,
	TAG_RENDERER,
	TAG_SMALL
} memtag_t;

typedef enum {
	h_high,
	h_low,
	h_dontcare
} ha_pref;


/*

--- low memory ----
server vm
server clipmap
---mark---
renderer initialization (shaders, etc)
UI vm
cgame vm
renderer map
renderer models

---free---

temp file loading
--- high memory ---

*/

#if defined(_DEBUG) && !defined(BSPC)
#define ZONE_DEBUG
#define HUNK_DEBUG
#endif

#ifdef ZONE_DEBUG
#define Z_TagMalloc(size, tag)			Z_TagMallocDebug(size, tag, #size, __FILE__, __LINE__)
#define Z_Malloc(size)					Z_MallocDebug(size, #size, __FILE__, __LINE__)
#define S_Malloc(size)					S_MallocDebug(size, #size, __FILE__, __LINE__)
void *Z_TagMallocDebug( int size, int tag, char *label, char *file, int line );	// NOT 0 filled memory
void *Z_MallocDebug( int size, char *label, char *file, int line );			// returns 0 filled memory
void *S_MallocDebug( int size, char *label, char *file, int line );			// returns 0 filled memory
#else
void *Z_TagMalloc( int size, int tag );	// NOT 0 filled memory
void *Z_Malloc( int size );			// returns 0 filled memory
void *S_Malloc( int size );			// NOT 0 filled memory only for small allocations
#endif
bool Z_Free( void *ptr );
int Z_AvailableMainMemory( void );
int Z_AvailableSmallMemory( void );
int Z_AvailableRenderMemory( void );
float Z_AvailableMainMemoryPercent();
float Z_AvailableSmallMemoryPercent();
float Z_AvailableRenderMemoryPercent();

void Hunk_Clear( void );
void* Hunk_AllocateTempMemory( int size );
void Hunk_FreeTempMemory( void *buf );
int	Hunk_MemoryRemaining( void );

void* Hunk_AllocateTempMemory_Main( int size );
void Hunk_FreeTempMemory_Main( void* buf );
int  Hunk_MemoryRemaining_Main( void );

void* Hunk_AllocateTempMemory_Other( int size );
void Hunk_FreeTempMemory_Other( void* buf );
int  Hunk_MemoryRemaining_Other( void );

void QMem_Init(size_t smallZoneM, size_t generalZoneM, size_t renderZoneM, size_t hunkSizeM, size_t hunkSizeOtherM);
void QMem_End(void);

void QMem_Info( void );