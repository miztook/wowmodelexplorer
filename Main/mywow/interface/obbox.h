#pragma once

#include "base.h"

template <class T>
class obbox3d
{
public:
	obbox3d() { clear(); }
	obbox3d(const obbox3d& obb) : 
	Center(obb.Center),
		XAxis(obb.XAxis),
		YAxis(obb.YAxis),
		ZAxis(obb.ZAxis),
		ExtX(obb.ExtX),
		ExtY(obb.ExtY),
		ExtZ(obb.ExtZ),
		Extents(obb.Extents) {}

public:		//	Attributes

	vector3d<T>	Center;
	vector3d<T>	XAxis;
	vector3d<T>	YAxis;
	vector3d<T>	ZAxis;
	vector3d<T>	ExtX;
	vector3d<T>	ExtY;
	vector3d<T>	ExtZ;
	vector3d<T>	Extents;

public:
	bool isPointIn(const vector3d<T>& v) const;

	void completeExtAxis()
	{
		ExtX = XAxis * Extents.X;
		ExtY = YAxis * Extents.Y;
		ExtZ = ZAxis * Extents.Z;
	}

	void clear();
	
	void build(const aabbox3d<T>& aabb);

	void getVertices(vector3d<T>* aVertPos, unsigned short* aIndices, bool bWire) const;
};

template <class T>
inline bool obbox3d<T>::isPointIn(const vector3d<T>& v) const
{
	vector3d<T> vd = v - Center;

	//	Transfrom point to obb space
	float d = DotProduct(XAxis, vd);
	if (d < -Extents.x || d > Extents.x)
		return false;

	d = DotProduct(YAxis, vd);
	if (d < -Extents.y || d > Extents.y)
		return false;

	d = DotProduct(ZAxis, vd);
	if (d < -Extents.z || d > Extents.z)
		return false;

	return true;
}

template <class T>
inline void obbox3d<T>::clear()
{
	memset(this, 0, sizeof (obbox3d<T>));
}

template <class T>
inline void obbox3d<T>::build(const aabbox3d<T>& aabb)
{
	Center	= aabb.Center;
	XAxis	= vector3d<T>::UnitX();
	YAxis	= vector3d<T>::UnitY();
	ZAxis	= vector3d<T>::UnitZ();
	Extents	= aabb.Extents;

	completeExtAxis();
}

template <class T>
inline void obbox3d<T>::getVertices(vector3d<T>* aVertPos, unsigned short* aIndices, bool bWire) const
{
	static unsigned short indexTriangle[] =
	{
		0, 1, 3, 3, 1, 2, 
		2, 1, 6, 6, 1, 5, 
		5, 1, 4, 4, 1, 0,
		0, 3, 4, 4, 3, 7, 
		3, 2, 7, 7, 2, 6, 
		7, 6, 4, 4, 6, 5
	};

	static unsigned short indexWire[] =
	{
		0, 1, 1, 2, 2, 3, 3, 0, 
		0, 4, 1, 5, 2, 6, 3, 7,
		4, 5, 5, 6, 6, 7, 7, 4
	};

	if (aVertPos)
	{
		//	Up 4 vertex;
		//	Left Up corner;
		aVertPos[0] = Center - ExtX + ExtY + ExtZ;
		//	right up corner;
		aVertPos[1] = aVertPos[0] + ExtX * 2.0f;
		//	right bottom corner;
		aVertPos[2] = aVertPos[1] - ExtZ * 2.0f;
		//	left bottom corner;
		aVertPos[3] = aVertPos[2] - ExtX * 2.0f;

		//	Down 4 vertex;
		//	Left up corner;
		aVertPos[4] = Center - ExtX - ExtY + ExtZ;
		//	right up corner;
		aVertPos[5] = aVertPos[4] + ExtX * 2.0f;
		//	right bottom corner;
		aVertPos[6] = aVertPos[5] - ExtZ * 2.0f;
		//	left bottom corner;
		aVertPos[7] = aVertPos[6] - ExtX * 2.0f;
	}

	if (aIndices)
	{
		if (bWire)
			memcpy(aIndices, indexWire, sizeof (unsigned short) * 24);
		else
			memcpy(aIndices, indexTriangle, sizeof (unsigned short) * 36);
	}
}
