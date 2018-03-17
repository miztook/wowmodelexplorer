#pragma once

#include "base.h"
#include "linklist.h"
#include "IVideoResource.h"

class IVertexBuffer : public IVideoResource
{
private:
	DISALLOW_COPY_AND_ASSIGN(IVertexBuffer);

public:
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
		releaseVideoResources();

		if (Clear)
			delete[] Vertices;
	}

protected:
	virtual bool buildVideoResources() override final;
	virtual void releaseVideoResources() override final;
	virtual bool hasVideoBuilt() const override final { return HWLink != nullptr; }

public:
	bool updateHWBuffer(u32 size);

	template <class T>
	void set(const T* vertices, E_VERTEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping)
	{
		//ASSERT(type == T::TYPE());

		Vertices = vertices;
		Type = type;
		Size = size;

		Mapping = mapping;
	}

public:
	void set(void* vertices, E_STREAM_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping);

public:
	LENTRY		Link;		

	const void*	Vertices;
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

class IIndexBuffer : public IVideoResource
{
private:
	DISALLOW_COPY_AND_ASSIGN(IIndexBuffer);

public:
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
		releaseVideoResources();

		if (Clear)
			delete[] Indices;
	}

public:
	void set(void* indices, E_INDEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping);

protected:
	virtual bool buildVideoResources() override final;
	virtual void releaseVideoResources() override final;
	virtual bool hasVideoBuilt() const override final { return HWLink != nullptr; }

public:
	bool updateHWBuffer(u32 size);

	template <class T>
	void set(const T* indices, E_INDEX_TYPE type, u32 size, E_MESHBUFFER_MAPPING mapping)
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