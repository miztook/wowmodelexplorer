#pragma once

#include "q_memory.h"

#include <stddef.h>
#include <assert.h>

template<typename T>
class qzone_nolock_allocator {
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
	//    convert an qzone_nolock_allocator<T, bool> to qzone_nolock_allocator<U, bool>

	template<typename U>
	struct rebind {
		typedef qzone_nolock_allocator<U> other;
	};

public : 
	inline qzone_nolock_allocator() { 
		m_default = sizeof(T) >= 512;
	}
	inline ~qzone_nolock_allocator() {}
	inline qzone_nolock_allocator(const qzone_nolock_allocator& a)
	{
		m_default = sizeof(T) >= 512;
	}

	template<typename U>
	inline qzone_nolock_allocator(const qzone_nolock_allocator<U>& a)
	{
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
			if (Z_AvailableSmallMemory() > 512)
				p = reinterpret_cast<pointer>(Z_TagMalloc(sizeof(T) * cnt, TAG_RENDERER));
		}

		if (p)
			return p;

		return reinterpret_cast<pointer>(::operator new(cnt * sizeof (T))); 
	}

	inline void deallocate(pointer p, size_type) { 

		bool ret = Z_Free(p);

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
	qzone_nolock_allocator<U>& operator=(const qzone_nolock_allocator<U>&) { return *this; }

	inline bool operator==(const qzone_nolock_allocator& a) const { return this==&a; }
	inline bool operator!=(const qzone_nolock_allocator& a) const { return this!=&a; }

private:
	bool m_default;
};    //    end of class qzone_nolock_allocator 
