#pragma once

#include "q_memory.h"

#include <cstddef>
#include <cassert>
#include <cstdlib>
#include "CSysGlobal.h"

void* qzone_malloc(size_t size);
void qzone_free(void* p);

template<typename T>
class qzone_allocator {
public : 
	//    typedefs

	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

public : 
	//    convert an qzone_allocator<T, bool> to qzone_allocator<U, bool>

	template<typename U>
	struct rebind {
		typedef qzone_allocator<U> other;
	};

public : 
	 qzone_allocator() { 
		m_small = sizeof(T) <= 4;
		m_default = sizeof(T) >= 512;
	}
	 ~qzone_allocator() {}
	 qzone_allocator(const qzone_allocator& a)
	{
		m_small = sizeof(T) <= 4;
		m_default = sizeof(T) >= 512;
	}

	template<typename U>
	 qzone_allocator(const qzone_allocator<U>& a)
	{
		m_small = sizeof(U) <= 4;
		m_default = sizeof(T) >= 512;
	}

	//    address

	 static pointer address(reference r) { return &r; }
	 static const_pointer address(const_reference r) { return &r; }

	//    memory allocation

	 pointer allocate(size_type cnt, const void* hint = 0) { 
		assert(cnt == 1);
		return reinterpret_cast<pointer>(qzone_malloc(sizeof(T) * cnt));
	}

	 static void deallocate(pointer p, size_type)
	 {
		 qzone_free(p);
	 }

	//    size
	 static size_type max_size() { 
		size_type n=(size_type)(-1)/sizeof(T);
		return (0<n?n:1);
	}

	//    construction/destruction

	 static void construct(pointer p) { new T(); }
	 static void construct(pointer p, const T& t) { new(p) T(t); }

#if __cplusplus >= 201103L
	 template <typename ... U> void construct(pointer p, U&&... t) { new (p)T(std::forward<U>(t)...); }
#endif

	 static void destroy(pointer p) { p->~T(); }

	qzone_allocator& operator=(const qzone_allocator& other) 
	{ 
		if (this != &other)
		{
			m_small = other.m_small;
			m_default = other.m_default;
		}
		return *this; 
	}

	 bool operator==(const qzone_allocator& a) const { return this==&a || (m_small == a.m_small && m_default == a.m_default); }
	 bool operator!=(const qzone_allocator& a) const { return (m_small != a.m_small && m_default != a.m_default); }

private:
	bool m_small;
	bool m_default;
};    //    end of class qzone_allocator 

inline void* qzone_malloc(size_t size)
{
#ifdef MW_PLATFORM_WINDOWS
	void* p = nullptr;
	if (size < 512)
	{
		if (size <= 4)
		{
			BEGIN_LOCK(&g_Globals.allocatorCS);
			if (Z_AvailableSmallMemory() > 4)
				p = Z_TagMalloc(size, TAG_SMALL);
			END_LOCK(&g_Globals.allocatorCS);
		}
		else
		{
			BEGIN_LOCK(&g_Globals.allocatorCS);
			if (Z_AvailableMainMemory() > 512)
				p = Z_TagMalloc(size, TAG_GENERAL);
			END_LOCK(&g_Globals.allocatorCS);
		}
	}

	if (p)
		return p;

#endif
	return malloc(size);
}

inline void qzone_free(void* p)
{
#ifdef MW_PLATFORM_WINDOWS
	bool ret;
	BEGIN_LOCK(&g_Globals.allocatorCS);
	ret = Z_Free(p);
	END_LOCK(&g_Globals.allocatorCS);

	if (!ret)
#endif
		free(p); 
}
