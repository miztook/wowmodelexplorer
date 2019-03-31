#pragma once

#include "base.h"
#include "core.h"

#define R32(color) (uint32_t)((color>>16) & 0xff)
#define G32(color) (uint32_t)((color>>8) & 0xff);
#define B32(color) (uint32_t)(color & 0xff)

#define R16(color) (uint16_t)((color>>10) & 0x1f)
#define G16(color) (uint16_t)((color>>5) & 0x1f)
#define B16(color) (uint16_t)(color & 0x1f)

struct SColor
{
public:
	//
	SColor() : color(0xffffffff){ }
	SColor ( uint32_t a, uint32_t r, uint32_t g, uint32_t b ) : color(((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)) {}
	SColor( uint32_t r, uint32_t g, uint32_t b ) : color(((0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)){}
	SColor(uint32_t clr) :color(clr){}

public:
	//
	 uint32_t getAlpha() const { return color>>24; }
	 uint32_t getRed() const { return (color>>16) & 0xff; }
	 uint32_t getGreen() const { return (color>>8) & 0xff; }
	 uint32_t getBlue() const { return color & 0xff; }

	void set(uint32_t a, uint32_t r, uint32_t g, uint32_t b) { color = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)); }

	SColor operator+(const SColor& other) const
	{
		return SColor(min_(getAlpha() + other.getAlpha(), 255u),
			min_(getRed() + other.getRed(), 255u),
			min_(getGreen() + other.getGreen(), 255u),
			min_(getBlue() + other.getBlue(), 255u));
	}

	bool operator==(const SColor& other) const { return other.color == color; }
	bool operator!=(const SColor& other) const { return other.color != color; }

	static SColor interpolate(const SColor& a, const SColor& b, float d, bool alpha)
	{
		d = clamp_(d, 0.f, 1.f);
		const float inv = 1.0f - d;

		return SColor( alpha ? (uint32_t)(a.getAlpha()*inv + b.getAlpha()*d) : 255,
			(uint32_t)(a.getRed()*inv + b.getRed()*d),
			(uint32_t)(a.getGreen()*inv + b.getGreen()*d),
			(uint32_t)(a.getBlue()*inv + b.getBlue()*d));
	}

	void setAlpha(uint32_t a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }
	void setRed(uint32_t r) { color = ((r & 0xff)<<16) | (color & 0xff00ffff); }
	void setGreen(uint32_t g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }
	void setBlue(uint32_t b) { color = (b & 0xff) | (color & 0xffffff00); }

	float getLuminance() const
	{
		return 0.3f*getRed() + 0.59f*getGreen() + 0.11f*getBlue();
	}

	uint32_t getAverage() const
	{
		return (getRed() + getGreen() + getBlue()) / 3;
	}

	static const SColor& White() { static SColor m(0xffffffff); return m; }
	static const SColor& Red() { static SColor m(0xffff0000); return m; }
	static const SColor& Green() { static SColor m(0xff00ff00); return m; }
	static const SColor& Blue() { static SColor m(0xff0000ff); return m; }
	static const SColor& Yellow() { static SColor m(0xffffff00); return m; }
	static const SColor& Black() { static SColor m(0xff000000); return m; }

	SColor toBGRA() const { return SColor(getAlpha(), getBlue(), getGreen(), getRed()); }

public:
	uint32_t	color;		//A8R8G8B8 Format

public:
	static uint16_t A8R8G8B8toA1R5G5B5(uint32_t color);
	static uint32_t A1R5G5B5toA8R8G8B8(uint16_t color);
	static uint16_t X8R8G8B8toA1R5G5B5(uint32_t color);
	static uint16_t A8R8G8B8toR5G6B5(uint32_t color);
	static uint32_t R5G6B5toA8R8G8B8(uint16_t color);
	static uint16_t R5G6B5toA1R5G5B5(uint16_t color);
	static uint16_t A1R5G5B5toR5G6B5(uint16_t color);
	static uint16_t RGB16(uint32_t r, uint32_t g, uint32_t b);
	static uint16_t RGBA16(uint32_t r, uint32_t g, uint32_t b, uint32_t a=0xff);
	static uint16_t RGB16from16(uint16_t r, uint16_t g, uint16_t b);
};

struct SColorf
{
public:
	SColorf() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }

	SColorf(vector3df v) : r(v.X), g(v.Y), b(v.Z), a(1.0f) {}
	SColorf(float r,float g, float b, float a=1.0f) : r(r), g(g), b(b), a(a) { }

	SColorf(SColor c)
	{
		const float inv = 1.0f / 255.0f;
		r = c.getRed() * inv;
		g = c.getGreen() * inv;
		b = c.getBlue() * inv;
		a = c.getAlpha() * inv;
	}

	SColor toSColor() const
	{
		return SColor((uint32_t)(a*255.0f), (uint32_t)(r*255.0f), (uint32_t)(g*255.0f), (uint32_t)(b*255.0f));
	}

	void set(float rr, float gg, float bb) {r = rr; g =gg; b = bb; }
	void set(float aa, float rr, float gg, float bb) {a = aa; r = rr; g =gg; b = bb; }

	static SColorf interpolate(const SColorf& a, const SColorf& b, float d, bool alpha)
	{
		d = clamp_(d, 0.f, 1.f);
		const float inv = 1.0f - d;

		return SColorf( 
			(a.getRed()*inv + b.getRed()*d),
			(a.getGreen()*inv + b.getGreen()*d),
			(a.getBlue()*inv + b.getBlue()*d),
			alpha ? (a.getAlpha()*inv + b.getAlpha()*d) : 1.0f);
	}

	void setAlpha(float v) { a = v; }
	void setRed(float v) { r = v; }
	void setGreen(float v) { g = v; }
	void setBlue(float v) { b = v; }

	float getAlpha() const { return a; }
	float getRed() const { return r; }
	float getGreen() const { return g; }
	float getBlue() const { return b; }

	bool equals(const SColorf& other, const float tolerance = (float)ROUNDING_ERROR_f32) const
	{
		return equals_(r, other.r) && equals_(g, other.g) && equals_(b, other.b) && equals_(a, other.a);
	}

	bool operator==(const SColorf& other) const
	{
		if ( this == &other ) return true;
		return equals(other);
	}

	bool operator!=(const SColorf& other) const { return !(*this == other ); }

	bool operator<(const SColorf& other) const
	{
		if(!equals_(r, other.r))
			return r - other.r < 0;
		else if (!equals_(g, other.g))
			return g - other.g < 0;
		else if (!equals_(b, other.b))
			return b - other.b < 0;
		else if (!equals_(a, other.a))
			return a - other.a < 0;

		return false;
	}

	SColorf operator*(const SColorf& other) const { return SColorf(r*other.r, g*other.g, b*other.b, a*other.a);}
	SColorf operator+(const SColorf& other) const { return SColorf(r+other.r, g+other.g, b+other.b, 1.0f); }

	float r;
	float g;
	float b;
	float a;
};

inline uint16_t SColor::A8R8G8B8toA1R5G5B5( uint32_t color )
{
	return (uint16_t)(( color & 0x80000000) >> 16|
		( color & 0x00F80000) >> 9 |
		( color & 0x0000F800) >> 6 |
		( color & 0x000000F8) >> 3);
}

inline uint32_t SColor::A1R5G5B5toA8R8G8B8( uint16_t color )
{
	return ( (( -( (int32_t) color & 0x00008000 ) >> (int32_t) 31 ) & 0xFF000000 ) |
		(( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
		(( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
		(( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2)
		);
}

inline uint16_t SColor::X8R8G8B8toA1R5G5B5(uint32_t color)
{
	return (uint16_t)(0x8000 |
		( color & 0x00F80000) >> 9 |
		( color & 0x0000F800) >> 6 |
		( color & 0x000000F8) >> 3);
}

inline uint16_t SColor::A8R8G8B8toR5G6B5( uint32_t color )
{
	return (uint16_t)(( color & 0x00F80000) >> 8 |
		( color & 0x0000FC00) >> 5 |
		( color & 0x000000F8) >> 3);
}

inline uint32_t SColor::R5G6B5toA8R8G8B8( uint16_t color )
{
	return 0xFF000000 |
		((color & 0xF800) << 8)|
		((color & 0x07E0) << 5)|
		((color & 0x001F) << 3);
}

inline uint16_t SColor::R5G6B5toA1R5G5B5( uint16_t color )
{
	return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
}

inline uint16_t SColor::A1R5G5B5toR5G6B5( uint16_t color )
{
	return (((color & 0x7FE0) << 1) | (color & 0x1F));
}

inline uint16_t SColor::RGB16( uint32_t r, uint32_t g, uint32_t b )
{
	return RGBA16(r,g,b);
}

inline uint16_t SColor::RGBA16( uint32_t r, uint32_t g, uint32_t b, uint32_t a/*=0xff*/ )
{
	return (uint16_t)((a & 0x80) << 8 |
		(r & 0xF8) << 7 |
		(g & 0xF8) << 2 |
		(b & 0xF8) >> 3);
}

inline uint16_t SColor::RGB16from16( uint16_t r, uint16_t g, uint16_t b )
{
	return (0x8000 |
		(r & 0x1F) << 10 |
		(g & 0x1F) << 5  |
		(b & 0x1F));
}