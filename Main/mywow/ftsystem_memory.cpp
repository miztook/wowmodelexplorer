#include "stdafx.h"
#include "mywow.h"

extern "C"  void* 
	ft_alloc( FT_Memory  memory,
	long       size )
{
	FT_UNUSED( memory );

	return malloc( size );
}

extern "C" void* 
	ft_realloc( FT_Memory  memory,
	long       cur_size,
	long       new_size,
	void*      block )
{
	FT_UNUSED( memory );
	FT_UNUSED( cur_size );

	return realloc( block, new_size );
}

extern "C" void 
	ft_free( FT_Memory  memory,
	void*      block )
{
	FT_UNUSED( memory );

	free( block );
}

/* documentation is in ftobjs.h */

extern "C" FT_Memory
	FT_New_Memory( void )
{
	FT_Memory  memory;

	memory = (FT_Memory)ft_smalloc( sizeof ( *memory ) );
	if ( memory )
	{
		memory->user    = 0;
		memory->alloc   = ft_alloc;
		memory->realloc = ft_realloc;
		memory->free    = ft_free;
#ifdef FT_DEBUG_MEMORY
		ft_mem_debug_init( memory );
#endif
	}

	return memory;
}


/* documentation is in ftobjs.h */

extern "C" void
	FT_Done_Memory( FT_Memory  memory )
{
#ifdef FT_DEBUG_MEMORY
	ft_mem_debug_done( memory );
#endif
	ft_sfree( memory );
}
