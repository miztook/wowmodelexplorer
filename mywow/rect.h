#pragma once

template <class T>
class rect
{
public:
	//
	rect() {}
	rect(T x, T y, T x2, T y2) : UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {}
	rect(const vector2d<T>& upperLeft, const vector2d<T>& lowerRight) : UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}

	//
	inline T getWidth() const { return LowerRightCorner.X - UpperLeftCorner.X; }
	inline T getHeight() const { return LowerRightCorner.Y - UpperLeftCorner.Y; }
	inline dimension2d<T> getSize() const { return dimension2d<T>(getWidth(), getHeight()); }

	void clipAgainst(const rect<T>& other);
	bool contains(const rect<T>& other) const;
	bool intersectsWithRect(const rect<T>& other) const;

public:
	vector2d<T>		UpperLeftCorner;
	vector2d<T>		LowerRightCorner;
};

template <class T>
inline bool rect<T>::contains( const rect<T>& other ) const
{
	return (other.LowerRightCorner.X <= LowerRightCorner.X && 
		other.LowerRightCorner.Y <= LowerRightCorner.Y &&
		other.UpperLeftCorner.X >= UpperLeftCorner.X &&
		other.UpperLeftCorner.Y >= UpperLeftCorner.Y );
}

template <class T>
inline bool rect<T>::intersectsWithRect( const rect<T>& other ) const
{
	return ( UpperLeftCorner.X < other.LowerRightCorner.X && 
		UpperLeftCorner.Y < other.LowerRightCorner.Y &&
		LowerRightCorner.X > other.UpperLeftCorner.X &&
		LowerRightCorner.Y > other.UpperLeftCorner.Y );
}

template <class T>
inline void rect<T>::clipAgainst(const rect<T>& other)
{
	if (other.LowerRightCorner.X < LowerRightCorner.X)
		LowerRightCorner.X = other.LowerRightCorner.X;
	if (other.LowerRightCorner.Y < LowerRightCorner.Y)
		LowerRightCorner.Y = other.LowerRightCorner.Y;

	if (other.UpperLeftCorner.X > UpperLeftCorner.X)
		UpperLeftCorner.X = other.UpperLeftCorner.X;
	if (other.UpperLeftCorner.Y > UpperLeftCorner.Y)
		UpperLeftCorner.Y = other.UpperLeftCorner.Y;

	// correct possible invalid rect resulting from clipping
	if (UpperLeftCorner.Y > LowerRightCorner.Y)
		UpperLeftCorner.Y = LowerRightCorner.Y;
	if (UpperLeftCorner.X > LowerRightCorner.X)
		UpperLeftCorner.X = LowerRightCorner.X;
}

typedef rect<f32> rectf;
typedef rect<s32> recti;
