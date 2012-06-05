#pragma once

#include <math.h>
#include <float.h>
#include "base.h"

//
//		math
//

#define IR(x)                           ((u32&)(x))
#define FR(x)                           ((f32&)(x))

const s32 ROUNDING_ERROR_S32 = 0;
const f32 ROUNDING_ERROR_f32 =  0.000001f;

const f32 PI = 3.14159265359f;

const f32 DEGTORAD = PI /180.0f;
const f32 RADTODEG = 180.0f / PI;

inline f32 radToDeg( f32 radians ){ return RADTODEG*radians; }
inline f32 degToRad( f32 degrees ){ return DEGTORAD*degrees; }

template<class T>
inline const T& min_(const T& a, const T& b)
{
	return a < b ? a : b;
}
template<class T>
inline const T& max_(const T& a, const T& b)
{
	return a < b ? b : a;
}
template<class T>
inline T abs_(const T& a)
{
	return a < (T)0 ? -a : a;
}
template<class T>
inline T lerp_(const T& a, const T& b, const f32 t)
{
	return (T)(a*(1.f-t)) + (b*t);
}
template <class T>
inline const T clamp_(const T& value, const T& low, const T& high)
{
	return min_ (max_(value,low), high);
}
template <class T>
inline void swap_(T& a, T& b)
{
	T c(a);
	a = b;
	b = c;
}
inline bool equals_(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_f32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool equals_(const s32 a, const s32 b, const s32 tolerance = ROUNDING_ERROR_S32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool equals_(const u32 a, const u32 b, const s32 tolerance = ROUNDING_ERROR_S32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool iszero_(const f32 a, const f32 tolerance = ROUNDING_ERROR_f32)
{
	return fabsf(a) <= tolerance;
}
inline f32 squareroot_(const f32 f)
{
	return sqrtf(f);
}
inline s32 squareroot_(const s32 f)
{
	return static_cast<s32>(squareroot_(static_cast<f32>(f)));
}
inline f32 reciprocal_squareroot_(const f32 f)
{
	if (equals_(f, 0.0f))
		return FLT_MAX;
	else
		return 1.f / sqrtf(f);
}
inline s32 reciprocal_squareroot_(const s32 x)
{
	return static_cast<s32>(reciprocal_squareroot_(static_cast<f32>(x)));
}
inline f32 reciprocal_( const f32 f )
{
	if (equals_(f, 0.0f))
		return FLT_MAX;
	else
		return 1.f / f;
}
inline s32 floor32_(f32 x)
{
	return (s32) floorf ( x );
}
inline s32 ceil32_( f32 x )
{
	return (s32) ceilf ( x );
}
inline f32 round_( f32 x )
{
	return floorf( x + 0.5f );
}
inline s32 round32_(f32 x)
{
	return (s32)round_(x);
}
inline f32 fract ( f32 x )
{
	return x - floorf ( x );
}

static int randSeed = 0;

inline void	srand_( unsigned seed ) {
	randSeed = seed;
}

inline int		rand_( void ) {
	randSeed = (69069 * randSeed + 1);
	return randSeed & 0x7fff;
}

inline float	frand( void )
{
	return rand_() / (float)0x7fff;
}

inline float	randfloat( float lower, float upper)
{
	return lower + (upper - lower) * frand();
}

inline int		randint( int lower, int upper)
{
	return lower + (int)((upper + 1 - lower) * frand());
}

//
//		includes
//

#include "dimension2d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "rect.h"
#include "triangle3d.h"
#include "line2d.h"
#include "line3d.h"
#include "plane3d.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "frustum.h"
#include "quaternion.h"
#include "coreutil.h"
#include "algorithm.h"
#include "animation.h"
#include "str.h"
#include "tracktexture.h"

struct  SDrawParam
{
	u32		voffset;
	u32		voffset2;
	s32		baseVertIndex;
	u32		minVertIndex;
	u32		numVertices;
	u32		startIndex;
};


struct SBoneMatrixArray
{
	SBoneMatrixArray(matrix4A16* mats, u8 size, u16 weights) : matrices(mats), count(size) 
	{
		maxWeights = clamp_(weights, (u16)1, (u16)4);
	}
	matrix4A16*		matrices;
	u8		count;
	u16		maxWeights;
};