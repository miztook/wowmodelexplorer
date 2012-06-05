#pragma once

#include "base.h"
#include "core.h"

#define R32(color) (u32)((color>>16) & 0xff)
#define G32(color) (u32)((color>>8) & 0xff);
#define B32(color) (u32)(color & 0xff)

#define R16(color) (u16)((color>>10) & 0x1f)
#define G16(color) (u16)((color>>5) & 0x1f)
#define B16(color) (u16)(color & 0x1f)

struct SColor
{
public:
	//
	SColor() : color(0xffffffff){ }
	SColor ( u32 a, u32 r, u32 g, u32 b ) : color(((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)) {}
	SColor( u32 r, u32 g, u32 b ) : color(((0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)){}
	SColor(u32 clr) :color(clr){}

public:
	//
	inline u32 getAlpha() const { return color>>24; }
	inline u32 getRed() const { return (color>>16) & 0xff; }
	inline u32 getGreen() const { return (color>>8) & 0xff; }
	inline u32 getBlue() const { return color & 0xff; }

	void set(u32 a, u32 r, u32 g, u32 b) { color = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)); }

	SColor operator+(const SColor& other) const
	{
		return SColor(min_(getAlpha() + other.getAlpha(), 255u),
			min_(getRed() + other.getRed(), 255u),
			min_(getGreen() + other.getGreen(), 255u),
			min_(getBlue() + other.getBlue(), 255u));
	}

	bool operator==(const SColor& other) const { return other.color == color; }
	bool operator!=(const SColor& other) const { return other.color != color; }

	static SColor interpolate(const SColor& a, const SColor& b, f32 d, bool alpha)
	{
		d = clamp_(d, 0.f, 1.f);
		const f32 inv = 1.0f - d;

		return SColor( alpha ? (u32)(a.getAlpha()*inv + b.getAlpha()*d) : 255,
			(u32)(a.getRed()*inv + b.getRed()*d),
			(u32)(a.getGreen()*inv + b.getGreen()*d),
			(u32)(a.getBlue()*inv + b.getBlue()*d));
	}

	void setAlpha(u32 a) { color = ((a & 0xff)<<24) | (color & 0x00ffffff); }
	void setRed(u32 r) { color = ((r & 0xff)<<16) | (color & 0xff00ffff); }
	void setGreen(u32 g) { color = ((g & 0xff)<<8) | (color & 0xffff00ff); }
	void setBlue(u32 b) { color = (b & 0xff) | (color & 0xffffff00); }

	f32 getLuminance() const
	{
		return 0.3f*getRed() + 0.59f*getGreen() + 0.11f*getBlue();
	}

	u32 getAverage() const
	{
		return (getRed() + getGreen() + getBlue()) / 3;
	}

public:
	u32	color;		//A8R8G8B8 Format

public:
	static u16 A8R8G8B8toA1R5G5B5(u32 color);
	static u32 A1R5G5B5toA8R8G8B8(u16 color);
	static u16 X8R8G8B8toA1R5G5B5(u32 color);
	static u16 A8R8G8B8toR5G6B5(u32 color);
	static u32 R5G6B5toA8R8G8B8(u16 color);
	static u16 R5G6B5toA1R5G5B5(u16 color);
	static u16 A1R5G5B5toR5G6B5(u16 color);
	static u16 RGB16(u32 r, u32 g, u32 b);
	static u16 RGBA16(u32 r, u32 g, u32 b, u32 a=0xff);
	static u16 RGB16from16(u16 r, u16 g, u16 b);
};

struct SColorf
{
public:
	SColorf() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }

	SColorf(f32 r,f32 g, f32 b, f32 a=1.0f) : r(r), g(g), b(b), a(a) { }

	SColorf(SColor c)
	{
		const f32 inv = 1.0f / 255.0f;
		r = c.getRed() * inv;
		g = c.getGreen() * inv;
		b = c.getBlue() * inv;
		a = c.getAlpha() * inv;
	}

	SColor toSColor() const
	{
		return SColor((u32)(a*255.0f), (u32)(r*255.0f), (u32)(g*255.0f), (u32)(b*255.0f));
	}

	void set(f32 rr, f32 gg, f32 bb) {r = rr; g =gg; b = bb; }
	void set(f32 aa, f32 rr, f32 gg, f32 bb) {a = aa; r = rr; g =gg; b = bb; }

	static SColorf interpolate(const SColorf& a, const SColorf& b, f32 d, bool alpha)
	{
		d = clamp_(d, 0.f, 1.f);
		const f32 inv = 1.0f - d;

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

	f32 getAlpha() const { return a; }
	f32 getRed() const { return r; }
	f32 getGreen() const { return g; }
	f32 getBlue() const { return b; }

	bool operator==(const SColorf& other) const
	{
		return equals_(r, other.r) && equals_(g, other.g) && equals_(b, other.b) && equals_(a, other.a);
	}

	bool operator!=(const SColorf& other) const { return !(*this == other ); }

	f32 r;
	f32 g;
	f32 b;
	f32 a;
};

inline u16 SColor::A8R8G8B8toA1R5G5B5( u32 color )
{
	return (u16)(( color & 0x80000000) >> 16|
		( color & 0x00F80000) >> 9 |
		( color & 0x0000F800) >> 6 |
		( color & 0x000000F8) >> 3);
}

inline u32 SColor::A1R5G5B5toA8R8G8B8( u16 color )
{
	return ( (( -( (s32) color & 0x00008000 ) >> (s32) 31 ) & 0xFF000000 ) |
		(( color & 0x00007C00 ) << 9) | (( color & 0x00007000 ) << 4) |
		(( color & 0x000003E0 ) << 6) | (( color & 0x00000380 ) << 1) |
		(( color & 0x0000001F ) << 3) | (( color & 0x0000001C ) >> 2)
		);
}

inline u16 SColor::X8R8G8B8toA1R5G5B5(u32 color)
{
	return (u16)(0x8000 |
		( color & 0x00F80000) >> 9 |
		( color & 0x0000F800) >> 6 |
		( color & 0x000000F8) >> 3);
}

inline u16 SColor::A8R8G8B8toR5G6B5( u32 color )
{
	return (u16)(( color & 0x00F80000) >> 8 |
		( color & 0x0000FC00) >> 5 |
		( color & 0x000000F8) >> 3);
}

inline u32 SColor::R5G6B5toA8R8G8B8( u16 color )
{
	return 0xFF000000 |
		((color & 0xF800) << 8)|
		((color & 0x07E0) << 5)|
		((color & 0x001F) << 3);
}

inline u16 SColor::R5G6B5toA1R5G5B5( u16 color )
{
	return 0x8000 | (((color & 0xFFC0) >> 1) | (color & 0x1F));
}

inline u16 SColor::A1R5G5B5toR5G6B5( u16 color )
{
	return (((color & 0x7FE0) << 1) | (color & 0x1F));
}

inline u16 SColor::RGB16( u32 r, u32 g, u32 b )
{
	return RGBA16(r,g,b);
}

inline u16 SColor::RGBA16( u32 r, u32 g, u32 b, u32 a/*=0xff*/ )
{
	return (u16)((a & 0x80) << 8 |
		(r & 0xF8) << 7 |
		(g & 0xF8) << 2 |
		(b & 0xF8) >> 3);
}

inline u16 SColor::RGB16from16( u16 r, u16 g, u16 b )
{
	return (0x8000 |
		(r & 0x1F) << 10 |
		(g & 0x1F) << 5  |
		(b & 0x1F));
}