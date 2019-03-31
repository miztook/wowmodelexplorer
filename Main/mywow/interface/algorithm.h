#pragma once

#include <ctype.h>

template<class T>
 inline T interpolate(const float r, const T &v1, const T &v2)
{
	return static_cast<T>(v1*(1.0f - r) + v2*r);
}

template<class T>
inline  T interpolateHermite(const float r, const T &v1, const T &v2, const T &in, const T &out)
{
	// basis functions
	float h1 = 2.0f*r*r*r - 3.0f*r*r + 1.0f;
	float h2 = -2.0f*r*r*r + 3.0f*r*r;
	float h3 = r*r*r - 2.0f*r*r + r;
	float h4 = r*r*r - r*r;

	// interpolation
	return static_cast<T>(v1*h1 + v2*h2 + in*h3 + out*h4);
}

template<class T>
 inline void heapsink(T* array, int32_t element, int32_t max)
{
	while ((element<<1) < max) // there is a left child
	{
		int32_t j = (element<<1);

		if (j+1 < max && array[j] < array[j+1])
			j = j+1; // take right child

		if (array[element] < array[j])
		{
			T t = array[j]; // swap elements
			array[j] = array[element];
			array[element] = t;
			element = j;
		}
		else
			return;
	}
}


template<class T>
 inline void heapsort(T* array_, int32_t size)
{
	// for heapsink we pretent this is not c++, where
	// arrays start with index 0. So we decrease the array pointer,
	// the maximum always +2 and the element always +1

	T* virtualArray = array_ - 1;
	int32_t virtualSize = size + 2;
	int32_t i;

	// build heap

	for (i=((size-1)/2); i>=0; --i)
		heapsink(virtualArray, i+1, virtualSize-1);

	// sort array

	for (i=size-1; i>=0; --i)
	{
		T t = array_[0];
		array_[0] = array_[i];
		array_[i] = t;
		heapsink(virtualArray, 1, i + 1);
	}
}


inline static uint32_t generateHashValue( const char *fname, const int size ) {
	int		i;
	uint32_t	hash;

	hash = 0;
	i = 0;
	while (fname[i] != '\0') {
		char letter = (char)tolower(fname[i]);
		if (letter =='.') break;				// don't include extension
		if (letter =='\\') letter = '/';		// damn path names
		hash+=(uint32_t)(letter)*(i+119);
		++i;
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20));
	hash &= (size-1);
	return hash;
}

inline static float KernelBSpline(const float x)
{
	if (x > 2.0f)
		return 0.0f;

	float xm1 = x - 1.0f;
	float xp1 = x + 1.0f;
	float xp2 = x + 2.0f;

	float a = (xp2 <= 0.0f) ? 0.0f : (xp2*xp2*xp2);
	float b = (xp1 <= 0.0f) ? 0.0f : (xp1*xp1*xp1);
	float c = (x <= 0) ? 0.0f : (x*x*x);
	float d = (xm1 <= 0.0f) ? 0.0f : (xm1*xm1*xm1);

	return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));
}