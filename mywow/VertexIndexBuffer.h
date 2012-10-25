#pragma once

#include "base.h"

class IVertexBuffer
{
private:
	DISALLOW_COPY_AND_ASSIGN(IVertexBuffer);

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
	void set(void* vertices, E_STREAM_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping);

	void setClear(bool c) { Clear = c; }

public:
	void* Vertices;
	E_STREAM_TYPE		Type;
	u32  Size;
	E_MESHBUFFER_MAPPING		Mapping;

	void*	HWLink;

private:
	bool Clear;
};

inline void IVertexBuffer::set( void* vertices, E_STREAM_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping )
{
	Vertices = vertices;
	Type = type;
	Size = size;

	Mapping = mapping;
}

class IIndexBuffer
{
private:
	DISALLOW_COPY_AND_ASSIGN(IIndexBuffer);

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

struct SBufferParam
{
	IVertexBuffer*		vbuffer0;				//1 stream
	IVertexBuffer*		vbuffer1;			//2 stream
	IVertexBuffer*		vbuffer2;			//3 stream
	IVertexBuffer*		vbuffer3;			//4 stream
	E_VERTEX_TYPE		vType;
	IIndexBuffer*		ibuffer;

	void clear()
	{
		vbuffer0 = vbuffer1 = vbuffer2 = vbuffer3 = NULL;
		ibuffer = NULL;
	}

	void destroy()
	{
		delete ibuffer; ibuffer = NULL;
		delete vbuffer3; vbuffer3 = NULL;
		delete vbuffer2; vbuffer2 = NULL;
		delete vbuffer1; vbuffer1 = NULL;
		delete vbuffer0; vbuffer0 = NULL;
	}

	IVertexBuffer* getVBuffer(u32 index) const
	{
		switch(index)
		{
		case 0:
			return vbuffer0;
		case 1:
			return vbuffer1;
		case 2:
			return vbuffer2;
		case 3:
			return vbuffer3;
		default:
			return NULL;
		}
	}

};