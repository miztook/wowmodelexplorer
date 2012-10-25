#pragma once

template <class T>
class dimension2d
{
public:
	//
	dimension2d(){}
	dimension2d(const T& width, const T& height) : Width(width), Height(height) {}
	dimension2d(const dimension2d<T>& other) : Width(other.Width), Height(other.Height) { }

	dimension2d<T>& operator=(const dimension2d<T>& other) { Width = other.Width; Height = other.Height; return *this; }
	//
	inline bool operator==(const dimension2d<T>& other) const { return equals_(Width, other.Width) && equals_(Height, other.Height); }
	inline bool operator!=(const dimension2d<T>& other) const { return !(*this == other ); }

	inline dimension2d<T>& operator/=(const T& scale) { Width /= scale; Height /= scale; return *this; }
	inline dimension2d<T> operator/(const T& scale) const { return dimension2d<T>(Width/scale, Height/scale); }

	inline dimension2d<T>& operator*=(const T& scale) { Width *= scale; 	Height *= scale; return *this; }
	inline dimension2d<T> operator*(const T& scale) const { return dimension2d<T>(Width*scale, Height*scale); }

	inline dimension2d<T>& operator+=(const dimension2d<T>& other) { Width += other.Width; Height += other.Height; return *this; }
	inline dimension2d<T> operator+(const dimension2d<T>& other) const { return dimension2d<T>(Width+other.Width, Height+other.Height); }

	inline dimension2d<T>& operator-=(const dimension2d<T>& other) { Width -= other.Width; Height -= other.Height; return *this; }
	inline dimension2d<T> operator-(const dimension2d<T>& other) const { return dimension2d<T>(Width-other.Width, Height-other.Height); }

	//
	inline dimension2d<T>& set(const T& width, const T& height) { Width = width; Height = height; return *this; }
	inline T getArea() const { return Width*Height; }

	inline dimension2d<T> getInterpolated(const dimension2d<T>& other, f32 d) const
	{
		T inv = (T) (1.0f - d);
		return dimension2d<T>(other.Width*inv + Width*d, other.Height*inv + Height*d);
	}

	dimension2d<T> getOptimalSize(
		bool requirePowerOfTwo=true,
		bool requireSquare=false,
		bool larger=true,
		u32 maxValue = 0) const;

public:
	T Width;
	T Height;
};

template <class T>
inline dimension2d<T> dimension2d<T>::getOptimalSize( bool requirePowerOfTwo/*=true*/, bool requireSquare/*=false*/, bool larger/*=true*/, u32 maxValue /*= 0*/ ) const
{
	u32 i=1;
	u32 j=1;
	if (requirePowerOfTwo)
	{
		while (i<(u32)Width)
			i<<=1;
		if (!larger && i!=1 && i!=(u32)Width)
			i>>=1;
		while (j<(u32)Height)
			j<<=1;
		if (!larger && j!=1 && j!=(u32)Height)
			j>>=1;
	}
	else
	{
		i=(u32)Width;
		j=(u32)Height;
	}

	if (requireSquare)
	{
		if ((larger && (i>j)) || (!larger && (i<j)))
			j=i;
		else
			i=j;
	}

	f32 f = j / (f32)i;
	if (f > 1)
	{
		if (j > maxValue)
		{
			j = maxValue;
			i = (u32)(maxValue / f);
		}
	}
	else
	{
		if (i > maxValue)
		{
			i = maxValue;
			j = (u32)(maxValue * f);
		}
	}

	return dimension2d<T>((T)i,(T)j);
}

typedef dimension2d<f32> dimension2df;
typedef dimension2d<u32> dimension2du;
typedef dimension2d<s32> dimension2di;