#pragma once

template <class T>
class vector2d
{
public:
	//
	vector2d(){}
	vector2d(T nx, T ny) : X(nx), Y(ny) {}
	explicit vector2d(T n) : X(n), Y(n) {}
	vector2d(const vector2d<T>& other) : X(other.X), Y(other.Y) {}

	//
	inline vector2d<T> operator-() const { return vector2d<T>(-X, -Y); }
	inline vector2d<T>& operator=(const vector2d<T>& other) { X = other.X; Y = other.Y; return *this; }

	inline vector2d<T> operator+(const vector2d<T>& other) const { return vector2d<T>(X + other.X, Y + other.Y); }
	inline vector2d<T>& operator+=(const vector2d<T>& other) { X+=other.X; Y+=other.Y; return *this; }

	inline vector2d<T> operator-(const vector2d<T>& other) const { return vector2d<T>(X - other.X, Y - other.Y); }
	inline vector2d<T>& operator-=(const vector2d<T>& other) { X-=other.X; Y-=other.Y; return *this; }

	inline vector2d<T> operator*(const vector2d<T>& other) const { return vector2d<T>(X * other.X, Y * other.Y); }
	inline vector2d<T>& operator*=(const vector2d<T>& other) { X*=other.X; Y*=other.Y; return *this; }
	inline vector2d<T> operator*(const T v) const { return vector2d<T>(X * v, Y * v); }
	inline vector2d<T>& operator*=(const T v) { X*=v; Y*=v; return *this; }

	inline vector2d<T> operator/(const vector2d<T>& other) const { return vector2d<T>(X / other.X, Y / other.Y); }
	inline vector2d<T>& operator/=(const vector2d<T>& other) { X/=other.X; Y/=other.Y; return *this; }
	inline vector2d<T> operator/(const T v) const { return vector2d<T>(X / v, Y / v); }
	inline vector2d<T>& operator/=(const T v) { X/=v; Y/=v; return *this; }

	inline bool operator==(const vector2d<T>& other) const { return equals_(other); }
	inline bool operator!=(const vector2d<T>& other) const { return !equals_(other); }

	//
	bool equals(const vector2d<T>& other) const { return equals_(X, other.X) && equals_(Y, other.Y); }

	void set(T nx, T ny) {X=nx; Y=ny;}
	void set(const vector2d<T>& p) { X=p.X; Y=p.Y; }

	T getLength() const { return squareroot_( X*X + Y*Y ); }
	T getLengthSQ() const { return X*X + Y*Y; }
	T dotProduct(const vector2d<T>& other) const { return X*other.X + Y*other.Y; }

	T getDistanceFrom(const vector2d<T>& other) const { return vector2d<T>(X - other.X, Y - other.Y).getLength(); }
	T getDistanceFromSQ(const vector2d<T>& other) const { return vector2d<T>(X - other.X, Y - other.Y).getLengthSQ(); }

	vector2d<T>& normalize()
	{
		f32 length = (f32)(X*X + Y*Y);
		if (equals_(length, 0.f))
			return *this;
		length = reciprocal_squareroot_( length );
		X = (T)(X * length);
		Y = (T)(Y * length);
		return *this;
	}

	static vector2d<T> interpolate(const vector2d<T>& a, const vector2d<T>& b, f32 d)
	{
		f32 inv = 1.0f - d;
		return vector2d<T>((a.X*inv + b.X*d), (a.Y*inv + b.Y*d));
	}

	vector2d<T> getInterpolated_quadratic(const vector2d<T>& v2, const vector2d<T>& v3, f32 d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const f32 inv = 1.0f - d;
		const f32 mul0 = inv * inv;
		const f32 mul1 = 2.0f * d * inv;
		const f32 mul2 = d * d;

		return vector2d<T> ( (T)(X * mul0 + v2.X * mul1 + v3.X * mul2),
			(T)(Y * mul0 + v2.Y * mul1 + v3.Y * mul2));
	}


public:
	T X;
	T Y;
};

typedef vector2d<f32> vector2df;
typedef vector2d<s32> vector2di;
