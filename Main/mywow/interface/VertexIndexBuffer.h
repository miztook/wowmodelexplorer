#pragma once

#include "base.h"
#include "linklist.h"
#include "IVideoResource.h"

class CVertexBuffer : public IVideoResource
{
private:
	DISALLOW_COPY_AND_ASSIGN(CVertexBuffer);

public:
	explicit CVertexBuffer(bool clear)
		: Clear(clear)
	{
		HWLink = nullptr;
		Vertices = nullptr;
		Type = EST_P;
		Size = 0;
		Mapping = EMM_STATIC;

		InitializeListHead(&Link);
	}

	virtual ~CVertexBuffer() 
	{
		releaseVideoResources();

		if (Clear)
			delete[] Vertices;
	}

protected:
	virtual bool buildVideoResources() override final;
	virtual void releaseVideoResources() override final;
	virtual bool hasVideoBuilt() const override final { return HWLink != nullptr; }

public:
	bool updateHWBuffer(uint32_t size);

	template <class T>
	void set(const T* vertices, E_VERTEX_TYPE type, uint32_t size, E_MESHBUFFER_MAPPING mapping)
	{
		//ASSERT(type == T::TYPE());

		Vertices = vertices;
		Type = type;
		Size = size;

		Mapping = mapping;
	}

public:
	void set(void* vertices, E_STREAM_TYPE type, uint32_t size, E_MESHBUFFER_MAPPING mapping);

public:
	LENTRY		Link;		

	const void*	Vertices;
	void*	HWLink;
	uint32_t  Size;
	E_STREAM_TYPE		Type;
	E_MESHBUFFER_MAPPING		Mapping;
	
private:
	bool Clear;
};

inline void CVertexBuffer::set( void* vertices, E_STREAM_TYPE type, uint32_t size, E_MESHBUFFER_MAPPING mapping )
{
	Vertices = vertices;
	Type = type;
	Size = size;

	Mapping = mapping;
}

class CIndexBuffer : public IVideoResource
{
private:
	DISALLOW_COPY_AND_ASSIGN(CIndexBuffer);

public:
	explicit CIndexBuffer(bool clear) 
		: Clear(clear) 
	{
		HWLink = nullptr;
		Indices = nullptr;
		Type = EIT_16BIT;
		Size = 0;
		Mapping = EMM_STATIC;

		InitializeListHead(&Link);
	}

	virtual ~CIndexBuffer() 
	{
		releaseVideoResources();

		if (Clear)
			delete[] Indices;
	}

public:
	void set(void* indices, E_INDEX_TYPE type, uint32_t size, E_MESHBUFFER_MAPPING mapping);

protected:
	virtual bool buildVideoResources() override final;
	virtual void releaseVideoResources() override final;
	virtual bool hasVideoBuilt() const override final { return HWLink != nullptr; }

public:
	bool updateHWBuffer(uint32_t size);

	template <class T>
	void set(const T* indices, E_INDEX_TYPE type, uint32_t size, E_MESHBUFFER_MAPPING mapping)
	{
		ASSERT((sizeof(T) == 2 && type == EIT_16BIT) || (sizeof(T) == 4 && type == EIT_32BIT));

		Indices = indices;
		Type = type;
		Size = size;

		Mapping = mapping;
	}

public:
	LENTRY		Link;		//
public:
	const void*	Indices;
	void*	HWLink;
	uint32_t  Size;
	E_INDEX_TYPE		Type;
	E_MESHBUFFER_MAPPING		Mapping;
		
private:
	bool Clear;

};

inline void CIndexBuffer::set( void* indices, E_INDEX_TYPE type, uint32_t size, E_MESHBUFFER_MAPPING mapping)
{
	Indices = indices;
	Type = type;
	Size = size;

	Mapping = mapping;
}

struct SBufferParam
{
	CVertexBuffer*		vbuffer0;				//1 stream
	CVertexBuffer*		vbuffer1;			//2 stream
	CIndexBuffer*		ibuffer;	
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

	CVertexBuffer* getVBuffer(uint32_t index) const
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