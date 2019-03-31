#pragma once

template <class T>
class dimension2d
{
public:
	//
	 dimension2d() : Width(0), Height(0) {}
	 dimension2d(const T& width, const T& height) : Width(width), Height(height) {}
	 dimension2d(const dimension2d<T>& other) { (*this) = other; }

	 dimension2d<T>& operator=(const dimension2d<T>& other) 
	{ 
		Width = other.Width; Height = other.Height; 
		return *this;
	}
	
	 bool equals(const dimension2d<T>& other, const T tolerance=(T)ROUNDING_ERROR_f32) const { return equals_(Width, other.Width, tolerance) && equals_(Height, other.Height, tolerance); }

	//
	 bool operator==(const dimension2d<T>& other) const 
	{
		if ( this == &other ) return true;
		return Width == other.Width && Height == other.Height;
	}

	 bool operator!=(const dimension2d<T>& other) const { return !(*this == other ); }

	 dimension2d<T>& operator/=(const T& scale) { Width /= scale; Height /= scale; return *this; }
	 dimension2d<T> operator/(const T& scale) const { return dimension2d<T>(Width/scale, Height/scale); }

	 dimension2d<T>& operator*=(const T& scale) { Width *= scale; 	Height *= scale; return *this; }
	 dimension2d<T> operator*(const T& scale) const { return dimension2d<T>(Width*scale, Height*scale); }

	 dimension2d<T>& operator+=(const dimension2d<T>& other) { Width += other.Width; Height += other.Height; return *this; }
	 dimension2d<T> operator+(const dimension2d<T>& other) const { return dimension2d<T>(Width+other.Width, Height+other.Height); }

	 dimension2d<T>& operator-=(const dimension2d<T>& other) { Width -= other.Width; Height -= other.Height; return *this; }
	 dimension2d<T> operator-(const dimension2d<T>& other) const { return dimension2d<T>(Width-other.Width, Height-other.Height); }

	//
	 dimension2d<T>& set(const T& width, const T& height) { Width = width; Height = height; return *this; }
	 T getArea() const { return Width*Height; }

	uint32_t getNumMipLevels() const;

	 dimension2d<T> getMipLevelSize(uint32_t level) const 
	{
		return dimension2d<T>(max_(1u, Width >> level), max_(1u, Height >> level)); 
	}

	dimension2d<T> getInterpolated(const dimension2d<T>& other, float d) const;
	
	dimension2d<T> getOptimalSize(
		bool requirePowerOfTwo=true,
		bool requireSquare=false,
		bool larger=true,
		uint32_t maxValue = 0) const;

public:
	T Width;
	T Height;
};

template <class T>
inline uint32_t dimension2d<T>::getNumMipLevels() const
{
	uint32_t mip = 0;
	uint32_t len = max_(Width, Height);
	while(len)
	{
		len >>= 1;
		++mip;
	}

	return mip;
}

template <class T>
inline dimension2d<T> dimension2d<T>::getInterpolated( const dimension2d<T>& other, float d ) const
{	
	const float inv = 1.0f - d;
	return dimension2d<T>(other.Width*inv + Width*d, other.Height*inv + Height*d);
}

template <class T>
inline dimension2d<T> dimension2d<T>::getOptimalSize( bool requirePowerOfTwo/*=true*/, bool requireSquare/*=false*/, bool larger/*=true*/, uint32_t maxValue /*= 0*/ ) const
{
	uint32_t i=1;
	uint32_t j=1;
	if (requirePowerOfTwo)
	{
		while (i<(uint32_t)Width)
			i<<=1;
		if (!larger && i!=1 && i!=(uint32_t)Width)
			i>>=1;
		while (j<(uint32_t)Height)
			j<<=1;
		if (!larger && j!=1 && j!=(uint32_t)Height)
			j>>=1;
	}
	else
	{
		i=(uint32_t)Width;
		j=(uint32_t)Height;
	}

	if (requireSquare)
	{
		if ((larger && (i>j)) || (!larger && (i<j)))
			j=i;
		else
			i=j;
	}

	float f = j / (float)i;
	if (f > 1)
	{
		if (j > maxValue)
		{
			j = maxValue;
			i = (uint32_t)(maxValue / f);
		}
	}
	else
	{
		if (i > maxValue)
		{
			i = maxValue;
			j = (uint32_t)(maxValue * f);
		}
	}

	return dimension2d<T>((T)i,(T)j);
}

typedef dimension2d<float> dimension2df;
typedef dimension2d<uint32_t> dimension2du;
typedef dimension2d<int32_t> dimension2di;
