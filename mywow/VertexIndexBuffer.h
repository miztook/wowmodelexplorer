#pragma once

#include "base.h"

class IVertexBuffer
{
public:
	IVertexBuffer() : HWLink(NULL), Clear(true) {}
	IVertexBuffer(bool clear)
		: HWLink(NULL), Clear(clear) {}

	virtual ~IVertexBuffer() 
	{
		if (Clear)
			delete[] Vertices;
	}

public:
	void set(void* vertices, E_VERTEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping);

	void setClear(bool c) { Clear = c; }

public:
	void* Vertices;
	E_VERTEX_TYPE		Type;
	u32  Size;
	E_MESHBUFFER_MAPPING		Mapping;

	void*	HWLink;

private:
	bool Clear;
};

inline void IVertexBuffer::set( void* vertices, E_VERTEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping )
{
	Vertices = vertices;
	Type = type;
	Size = size;


	Mapping = mapping;

}

class IIndexBuffer
{
public:
	IIndexBuffer() : HWLink(NULL), Clear(true) {}
	IIndexBuffer(bool clear) 
		: HWLink(NULL), Clear(clear) { }

	virtual ~IIndexBuffer() 
	{
		if (Clear)
			delete[] Indices;
	}

public:
	void set(void* indices, E_INDEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping);

	void setClear(bool c) { Clear = c; }

public:
	void*	Indices;
	E_INDEX_TYPE		Type;
	u32  Size;
	E_MESHBUFFER_MAPPING		Mapping;

	void*	HWLink;

private:
	bool Clear;
};

inline void IIndexBuffer::set( void* indices, E_INDEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping)
{
	Indices = indices;
	Type = type;
	Size = size;

	Mapping = mapping;
}
