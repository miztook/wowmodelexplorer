#pragma once

#include "base.h"

class IVertexBuffer
{
private:
	DISALLOW_COPY_AND_ASSIGN(IVertexBuffer);

public:
	IVertexBuffer()
	{
		HWLink = nullptr;
		Clear = true;
		Vertices = nullptr;
		Type = EST_P;
		Size = 0;
		Mapping = EMM_STATIC;

		InitializeListHead(&Link);
	}
	explicit IVertexBuffer(bool clear)
		: Clear(clear)
	{
		HWLink = nullptr;
		Vertices = nullptr;
		Type = EST_P;
		Size = 0;
		Mapping = EMM_STATIC;

		InitializeListHead(&Link);
	}

	virtual ~IVertexBuffer() 
	{
		if (Clear)
			delete[] Vertices;
	}

public:
	void set(void* vertices, E_STREAM_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping);

	void setClear(bool c) { Clear = c; }

public:
	LENTRY		Link;		

	void*	Vertices;
	void*	HWLink;
	u32  Size;
	E_STREAM_TYPE		Type;
	E_MESHBUFFER_MAPPING		Mapping;
	
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
	IIndexBuffer() 
	{
		HWLink = nullptr;
		Clear = true;
		Indices = nullptr;
		Type = EIT_16BIT;
		Size = 0;
		Mapping = EMM_STATIC;

		InitializeListHead(&Link);
	}
	explicit IIndexBuffer(bool clear) 
		: Clear(clear) 
	{
		HWLink = nullptr;
		Indices = nullptr;
		Type = EIT_16BIT;
		Size = 0;
		Mapping = EMM_STATIC;

		InitializeListHead(&Link);
	}

	virtual ~IIndexBuffer() 
	{
		if (Clear)
			delete[] Indices;
	}

public:
	void set(void* indices, E_INDEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping);

	void setClear(bool c) { Clear = c; }
public:
	LENTRY		Link;		//
public:
	void*	Indices;
	void*	HWLink;
	u32  Size;
	E_INDEX_TYPE		Type;
	E_MESHBUFFER_MAPPING		Mapping;
		
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
	IIndexBuffer*		ibuffer;	
	E_VERTEX_TYPE		vType;

	void clear()
	{
		vbuffer0 = vbuffer1 = nullptr;
		ibuffer = nullptr;
	}

	void destroy()
	{
		delete ibuffer; ibuffer = nullptr;
		delete vbuffer1; vbuffer1 = nullptr;
		delete vbuffer0; vbuffer0 = nullptr;
	}

	IVertexBuffer* getVBuffer(u32 index) const
	{
		switch(index)
		{
		case 0:
			return vbuffer0;
		case 1:
			return vbuffer1;
		default:
			return nullptr;
		}
	}

};