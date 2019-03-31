#pragma once

template <class T>
class triangle3d
{
public:
	//
	 triangle3d() {}
	 triangle3d(vector3d<T> v1, vector3d<T> v2, vector3d<T> v3) : pointA(v1), pointB(v2), pointC(v3) {}
	 triangle3d(const triangle3d<T>& other) { *this = other; }

	 triangle3d& operator=(const triangle3d<T>& other)
	{
		ASSERT(this != &other);
		pointA = other.pointA;
		pointB = other.pointB;
		pointC = other.pointC;
		return *this;
	}

	 bool equals(const triangle3d<T>& other, const T tolerance = (T)ROUNDING_ERROR_f32) const { return equals_(pointA, other.pointA) && equals_(pointB, other.pointB) && equals_(other.pointC); }

	bool operator==(const triangle3d<T>& other) const 
	{
		if ( this == &other ) return true;
		return pointA == other.pointA && pointB == other.pointB && pointC == other.pointC;
	}

	 bool operator!=(const triangle3d<T>& other) const { return !(*this==other); }

	 vector3d<T> getNormal() const { return (pointB - pointA).crossProduct(pointC - pointA); }

	bool getIntersectionWithLine(const vector3d<T>& linePoint,
		const vector3d<T>& lineVect, vector3d<T>& outIntersection) const;

	bool getIntersectionOfPlaneWithLine(const vector3d<T>& linePoint,
		const vector3d<T>& lineVect, vector3d<T>& outIntersection) const;

	bool isPointInside(const vector3d<T>& p) const;

public:
	vector3d<T> pointA;
	vector3d<T> pointB;
	vector3d<T> pointC;

};

template <class T>
inline bool triangle3d<T>::getIntersectionWithLine( const vector3d<T>& linePoint, const vector3d<T>& lineVect, vector3d<T>& outIntersection ) const
{
	if (getIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
		return isPointInside(outIntersection);

	return false;
}

template <class T>
inline bool triangle3d<T>::getIntersectionOfPlaneWithLine( const vector3d<T>& linePoint, const vector3d<T>& lineVect, vector3d<T>& outIntersection ) const
{
	const vector3d<T> normal = getNormal().normalize();
	T t2;

	if ( iszero_( t2 = normal.dotProduct(lineVect) ) )
		return false;

	T d = pointA.dotProduct(normal);
	T t = -(normal.dotProduct(linePoint) - d) / t2;
	outIntersection = linePoint + (lineVect * t);
	return true;
}

template <class T>
inline bool triangle3d<T>::isPointInside( const vector3d<T>& p ) const
{
	const vector3d<T> f = pointB - pointA;
	const vector3d<T> g = pointC - pointA;

	const float a = f.dotProduct(f);
	const float b = f.dotProduct(g);
	const float c = g.dotProduct(g);

	const vector3d<T> vp = p - pointA;
	const float d = vp.dotProduct(f);
	const float e = vp.dotProduct(g);

	float x = (d*c)-(e*b);
	float y = (e*a)-(d*b);
	const float ac_bb = (a*c)-(b*b);
	float z = x+y-ac_bb;

	return (( (IR(z)) & ~((IR(x))|(IR(y))) ) & 0x80000000)!=0;
}

typedef triangle3d<float>	triangle3df;
typedef triangle3d<int32_t>	triangle3di;
