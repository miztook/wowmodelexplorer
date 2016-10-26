#include "stdafx.h"
#include "mywow.h"
#include "wowEnvironment.h"

struct SPointer
{
	IReadFile* file;
	IMemFile* mpqfile;
};

FT_CALLBACK_DEF( unsigned long )
	ft_mpq_stream_io( FT_Stream       stream,
	unsigned long   offset,
	unsigned char*  buffer,
	unsigned long   count )
{
	SPointer* pointer;

	if ( !count && offset > stream->size )
		return 1;

	pointer = (SPointer*)stream->descriptor.pointer;
	ASSERT(pointer);

	if (pointer->file)
	{
		IReadFile* file = pointer->file;

		if ( stream->pos != offset )
			file->seek((s32)offset, false);

		return (unsigned long)file->read(buffer, (u32)count);
	}
	else
	{
		IMemFile* file = pointer->mpqfile;

		if ( stream->pos != offset )
			file->seek((s32)offset, false);

		return (unsigned long)file->read(buffer, (u32)count);
	}
}

FT_CALLBACK_DEF( void )
	ft_mpq_stream_close( FT_Stream  stream )
{
	SPointer* pointer = (SPointer*)stream->descriptor.pointer;
	if (pointer->file)
	{
		delete pointer->file;
	}
	else
	{
		pointer->mpqfile->close();
		delete pointer->mpqfile;
	}
	delete pointer;
	stream->descriptor.pointer = NULL_PTR;
	stream->size               = 0;
	stream->base               = 0;
}

/* documentation is in ftstream.h */

FT_CALLBACK_DEF( FT_Error )
	FT_Stream_Open( FT_Stream    stream,
	const char*  filepathname )
{	
	if ( !stream )
		return FT_Err_Invalid_Stream_Handle;

	SPointer* pointer = new SPointer;
	pointer->file = NULL_PTR;
	pointer->mpqfile = NULL_PTR;

	stream->descriptor.pointer = NULL_PTR;
	stream->pathname.pointer   = (char*)filepathname;
	stream->base               = 0;
	stream->pos                = 0;
	stream->read               = NULL_PTR;
	stream->close              = NULL_PTR;

	const c8* basedir = g_Engine->getFileSystem()->getBaseDirectory();
	c8 path[QMAX_PATH];
	Q_strcpy(path, QMAX_PATH, basedir);
	normalizeDirName(path);
	Q_strcat(path, QMAX_PATH, filepathname);
	
	IReadFile* file = g_Engine->getFileSystem()->createAndOpenFile(path, true);
	if (file)
	{
		pointer->file = file;

		stream->size = file->getSize();
	}
	else
	{
		pointer->mpqfile = g_Engine->getWowEnvironment()->openFile(filepathname, false);

		if ( !pointer->mpqfile )
		{
			ASSERT(false);
			return FT_Err_Cannot_Open_Resource;
		}

		stream->size = pointer->mpqfile->getSize();
	}
	
	stream->descriptor.pointer = pointer;
	stream->read  = ft_mpq_stream_io;
	stream->close = ft_mpq_stream_close;

	return FT_Err_Ok;
}