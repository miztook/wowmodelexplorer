#pragma once

template <class T>
class vector3d
{
public:
	//
	vector3d(){}
	vector3d(T nx, T ny, T nz) : X(nx), Y(ny), Z(nz) {}
	explicit vector3d(T n) : X(n), Y(n), Z(n) {}
	vector3d(const vector3d<T>& other) : X(other.X), Y(other.Y), Z(other.Z) {}

	//
	inline vector3d<T> operator-() const { return vector3d<T>(-X, -Y, -Z); }

	inline vector3d<T>& operator=(const vector3d<T>& other) { X = other.X; Y = other.Y; Z = other.Z; return *this; }

	inline vector3d<T> operator+(const vector3d<T>& other) const { return vector3d<T>(X + other.X, Y + other.Y, Z + other.Z); }
	inline vector3d<T>& operator+=(const vector3d<T>& other) { X+=other.X; Y+=other.Y; Z+=other.Z; return *this; }

	inline vector3d<T> operator-(const vector3d<T>& other) const { return vector3d<T>(X - other.X, Y - other.Y, Z - other.Z); }
	inline vector3d<T>& operator-=(const vector3d<T>& other) { X-=other.X; Y-=other.Y; Z-=other.Z; return *this; }

	inline vector3d<T> operator*(const vector3d<T>& other) const { return vector3d<T>(X * other.X, Y * other.Y, Z * other.Z); }
	inline vector3d<T>& operator*=(const vector3d<T>& other) { X*=other.X; Y*=other.Y; Z*=other.Z; return *this; }
	inline vector3d<T> operator*(const T v) const { return vector3d<T>(X * v, Y * v, Z * v); }
	inline vector3d<T>& operator*=(const T v) { X*=v; Y*=v; Z*=v; return *this; }

	inline vector3d<T> operator/(const vector3d<T>& other) const { return vector3d<T>(X / other.X, Y / other.Y, Z / other.Z); }
	inline vector3d<T>& operator/=(const vector3d<T>& other) { X/=other.X; Y/=other.Y; Z/=other.Z; return *this; }
	inline vector3d<T> operator/(const T v) const { T i=(T)1.0/v; return vector3d<T>(X * i, Y * i, Z * i); }
	inline vector3d<T>& operator/=(const T v) { T i=(T)1.0/v; X*=i; Y*=i; Z*=i; return *this; }

	inline bool operator==(const vector3d<T>& other) const { return equals(other); }

	inline bool operator!=(const vector3d<T>& other) const { return !equals(other);	}

	//
	bool equals(const vector3d<T>& other, const T tolerance = (T)ROUNDING_ERROR_f32 ) const
	{
		return equals_(X, other.X, tolerance) &&
			equals_(Y, other.Y, tolerance) &&
			equals_(Z, other.Z, tolerance);
	}

	void set(const T nx, const T ny, const T nz) {X=nx; Y=ny; Z=nz;}
	void set(const vector3d<T>& p) {X=p.X; Y=p.Y; Z=p.Z;}
	void clear() { memset(&X,0,sizeof(T)*3); }

	T getLength() const { return squareroot_( X*X + Y*Y + Z*Z ); }
	T getLengthSQ() const { return X*X + Y*Y + Z*Z; }

	T dotProduct(const vector3d<T>& other) const { return X*other.X + Y*other.Y + Z*other.Z; }
	vector3d<T> crossProduct(const vector3d<T>& p) const { return vector3d<T>(Y * p.Z - Z * p.Y, Z * p.X - X * p.Z, X * p.Y - Y * p.X); }

	T getDistanceFrom(const vector3d<T>& other) const { return vector3d<T>(X - other.X, Y - other.Y, Z - other.Z).getLength(); }
	T getDistanceFromSQ(const vector3d<T>& other) const { return vector3d<T>(X - other.X, Y - other.Y, Z - other.Z).getLengthSQ(); }

	vector3d<T>& normalize()
	{
		f32 length = X*X + Y*Y + Z*Z;
		if (equals_(length, 0.0) || equals_(length, 1.0)) // this check isn't an optimization but prevents getting NAN in the sqrt.
			return *this;
		length = reciprocal_squareroot_(length);

		X = (T)(X * length);
		Y = (T)(Y * length);
		Z = (T)(Z * length);
		return *this;
	}

	static vector3d<T> interpolate(const vector3d<T>& a, const vector3d<T>& b, f32 d)
	{
		f32 inv = 1.0f - d;
		return vector3d<T>((a.X*inv + b.X*d), (a.Y*inv + b.Y*d), (a.Z*inv + b.Z*d));
	}

	vector3d<T> getInterpolated_quadratic(const vector3d<T>& v2, const vector3d<T>& v3, f32 d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const f32 inv = (T) 1.0 - d;
		const f32 mul0 = inv * inv;
		const f32 mul1 = (T) 2.0 * d * inv;
		const f32 mul2 = d * d;

		return vector3d<T> ((T)(X * mul0 + v2.X * mul1 + v3.X * mul2),
			(T)(Y * mul0 + v2.Y * mul1 + v3.Y * mul2),
			(T)(Z * mul0 + v2.Z * mul1 + v3.Z * mul2));
	}

public:
	T X;
	T Y;
	T Z;
};


typedef vector3d<f32> vector3df;
typedef vector3d<s32> vector3di;
