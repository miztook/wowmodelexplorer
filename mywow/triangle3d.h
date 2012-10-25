#pragma once

template <class T>
class triangle3d
{
public:
	//
	triangle3d() {}
	triangle3d(vector3d<T> v1, vector3d<T> v2, vector3d<T> v3) : pointA(v1), pointB(v2), pointC(v3) {}

	inline bool operator==(const triangle3d<T>& other) const { return equals_(pointA, other.pointA) && equals_(pointB, other.pointB) && equals_(other.pointC); }
	inline bool operator!=(const triangle3d<T>& other) const { return !(*this==other); }

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
bool triangle3d<T>::getIntersectionWithLine( const vector3d<T>& linePoint, const vector3d<T>& lineVect, vector3d<T>& outIntersection ) const
{
	if (getIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
		return isPointInside(outIntersection);

	return false;
}

template <class T>
bool triangle3d<T>::getIntersectionOfPlaneWithLine( const vector3d<T>& linePoint, const vector3d<T>& lineVect, vector3d<T>& outIntersection ) const
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
bool triangle3d<T>::isPointInside( const vector3d<T>& p ) const
{
	const vector3d<T> f = pointB - pointA;
	const vector3d<T> g = pointC - pointA;

	const f32 a = f.dotProduct(f);
	const f32 b = f.dotProduct(g);
	const f32 c = g.dotProduct(g);

	const vector3d<T> vp = p - pointA;
	const f32 d = vp.dotProduct(f);
	const f32 e = vp.dotProduct(g);

	f32 x = (d*c)-(e*b);
	f32 y = (e*a)-(d*b);
	const f32 ac_bb = (a*c)-(b*b);
	f32 z = x+y-ac_bb;

	return (( (IR(z)) & ~((IR(x))|(IR(y))) ) & 0x80000000)!=0;
}

typedef triangle3d<f32>	triangle3df;
typedef triangle3d<s32>	triangle3di;
