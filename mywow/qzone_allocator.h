#pragma once

#include "q_memory.h"

#include <stddef.h>
#include <assert.h>
#include <windows.h>

extern CRITICAL_SECTION g_allocatorCS;

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
	inline qzone_allocator() { 
		m_small = sizeof(T) <= 4;
		m_default = sizeof(T) >= 512;
	}
	inline ~qzone_allocator() {}
	inline qzone_allocator(const qzone_allocator& a)
	{
		m_small = sizeof(T) <= 4;
		m_default = sizeof(T) >= 512;
	}

	template<typename U>
	inline qzone_allocator(const qzone_allocator<U>& a)
	{
		m_small = sizeof(U) <= 4;
		m_default = sizeof(T) >= 512;
	}

	//    address

	inline pointer address(reference r) const { return &r; }
	inline const_pointer address(const_reference r) const { return &r; }

	//    memory allocation

	inline pointer allocate(size_type cnt, const void* hint = 0) { 
		_ASSERT(cnt == 1);
		pointer p = NULL;

		if (!m_default)
		{
			if (m_small)
			{
				::EnterCriticalSection(&g_allocatorCS);
				if (Z_AvailableSmallMemory() > 4)
					p = reinterpret_cast<pointer>(Z_TagMalloc(sizeof(T) * cnt, TAG_SMALL));
				::LeaveCriticalSection(&g_allocatorCS);
			}
			else
			{
				::EnterCriticalSection(&g_allocatorCS);
				if (Z_AvailableMainMemory() > 512)
					p = reinterpret_cast<pointer>(Z_TagMalloc(sizeof(T) * cnt, TAG_GENERAL));
				::LeaveCriticalSection(&g_allocatorCS);
			}
		}
			
		if (p)
			return p;

		return reinterpret_cast<pointer>(::operator new(cnt * sizeof (T))); 
	}

	inline void deallocate(pointer p, size_type) { 

		bool ret;
		::EnterCriticalSection(&g_allocatorCS);
		ret = Z_Free(p);
		::LeaveCriticalSection(&g_allocatorCS);
		
		if (!ret)
			::operator delete(p); 
	}

	//    size
	inline size_type max_size() const { 
		size_type n=(size_type)(-1)/sizeof(T);
		return (0<n?n:1);
	}

	//    construction/destruction

	inline void construct(pointer p, const T& t) { new(p) T(t); }
	inline void destroy(pointer p) { p->~T(); }

	template<typename U>
	qzone_allocator<U>& operator=(const qzone_allocator<U>&) { return *this; }

	inline bool operator==(const qzone_allocator& a) const { return this==&a; }
	inline bool operator!=(const qzone_allocator& a) const { return this!=&a; }

private:
	bool m_small;
	bool m_default;
};    //    end of class qzone_allocator 
