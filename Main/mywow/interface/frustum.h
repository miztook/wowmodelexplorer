#pragma once

#include <memory.h>
#include "plane3d.h"

enum VFPLANES : int32_t
{
	VF_NEAR_PLANE = 0,
	VF_FAR_PLANE,
	VF_LEFT_PLANE,
	VF_RIGHT_PLANE,
	VF_TOP_PLANE,
	VF_BOTTOM_PLANE,	
	VF_PLANE_COUNT,

	VF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

enum VFCORNERS: int32_t
{
	VFC_NEAR_RIGHT_TOP = 0,
	VFC_NEAR_LEFT_TOP,
	VFC_NEAR_LEFT_BOTTOM,
	VFC_NEAR_RIGHT_BOTTOM,

	VFC_FAR_RIGHT_TOP,
	VFC_FAR_LEFT_TOP,
	VFC_FAR_LEFT_BOTTOM,
	VFC_FAR_RIGHT_BOTTOM,
};

class frustum
{
public:
	 frustum() {}

	 frustum(const frustum& other) { (*this) = other; }
	 explicit frustum(const matrix4& mat) { setFrom(mat); }

public:
	frustum& operator=(const frustum& other)
	{
		ASSERT(this != &other);
		for (u32 i=0; i<VF_PLANE_COUNT; ++i)
			planes[i] = other.planes[i];
		return *this;
	}

	void setFrom( const vector3df& eye, 
		const vector3df& leftTop, 
		const vector3df& leftBottom, 
		const vector3df& rightTop, 
		const vector3df& rightBottom);

	void setFrom( const matrix4& mat );
	void transform( const matrix4& mat );

	static void makePlane( const matrix4& mat, VFPLANES side, plane3df& plane);

	bool isInFrustum( const aabbox3df& box ) const;
	bool isInFrustum( const vector3df& pos ) const;

	 bool operator==(const frustum& other) const { return equals(other); }
	 bool operator!=(const frustum& other) const { return !(*this == other); }

	 const plane3df& getPlane(VFPLANES index) const { return planes[index]; }
	 void setFarPlane(const plane3df& p) { planes[VF_FAR_PLANE] = p; }

	bool equals(const frustum& other) const;

private:
	plane3df		planes[VF_PLANE_COUNT];

};

inline void frustum::transform( const matrix4& mat )
{
	for (u32 i=0; i<VF_PLANE_COUNT; ++i)
		mat.transformPlane(planes[i]);
}

inline void frustum::setFrom( const matrix4& mat )
{
	// left clipping plane
	planes[VF_LEFT_PLANE].Normal.X = mat[3 ] + mat[0];
	planes[VF_LEFT_PLANE].Normal.Y = mat[7 ] + mat[4];
	planes[VF_LEFT_PLANE].Normal.Z = mat[11] + mat[8];
	planes[VF_LEFT_PLANE].D =        mat[15] + mat[12];

	// right clipping plane
	planes[VF_RIGHT_PLANE].Normal.X = mat[3 ] - mat[0];
	planes[VF_RIGHT_PLANE].Normal.Y = mat[7 ] - mat[4];
	planes[VF_RIGHT_PLANE].Normal.Z = mat[11] - mat[8];
	planes[VF_RIGHT_PLANE].D =        mat[15] - mat[12];

	// top clipping plane
	planes[VF_TOP_PLANE].Normal.X = mat[3 ] - mat[1];
	planes[VF_TOP_PLANE].Normal.Y = mat[7 ] - mat[5];
	planes[VF_TOP_PLANE].Normal.Z = mat[11] - mat[9];
	planes[VF_TOP_PLANE].D =        mat[15] - mat[13];

	// bottom clipping plane
	planes[VF_BOTTOM_PLANE].Normal.X = mat[3 ] + mat[1];
	planes[VF_BOTTOM_PLANE].Normal.Y = mat[7 ] + mat[5];
	planes[VF_BOTTOM_PLANE].Normal.Z = mat[11] + mat[9];
	planes[VF_BOTTOM_PLANE].D =        mat[15] + mat[13];

	// far clipping plane
	planes[VF_FAR_PLANE].Normal.X = mat[3 ] - mat[2];
	planes[VF_FAR_PLANE].Normal.Y = mat[7 ] - mat[6];
	planes[VF_FAR_PLANE].Normal.Z = mat[11] - mat[10];
	planes[VF_FAR_PLANE].D =        mat[15] - mat[14];

	// near clipping plane
	planes[VF_NEAR_PLANE].Normal.X = mat[2];
	planes[VF_NEAR_PLANE].Normal.Y = mat[6];
	planes[VF_NEAR_PLANE].Normal.Z = mat[10];
	planes[VF_NEAR_PLANE].D =        mat[14];

	// normalize normals
	u32 i;
	for ( i=0; i != VF_PLANE_COUNT; ++i)
	{
		planes[i].normalize();
	}
}

inline void frustum::setFrom( const vector3df& eye, const vector3df& leftTop, const vector3df& leftBottom, const vector3df& rightTop, const vector3df& rightBottom )
{
	planes[VF_LEFT_PLANE].setPlane(eye, leftTop, leftBottom);
	planes[VF_RIGHT_PLANE].setPlane(eye, rightBottom, rightTop);
	planes[VF_TOP_PLANE].setPlane(eye, rightTop, leftTop);
	planes[VF_BOTTOM_PLANE].setPlane(eye, leftBottom, rightBottom);
}

inline bool frustum::isInFrustum( const aabbox3df& box ) const
{
	for( int p=1; p<6; ++p )
	{
		if (ISREL3D_BACK == box.classifyPlaneRelation(planes[p]))
			return false;
	}

	return true;
}

inline bool frustum::isInFrustum( const vector3df& pos ) const
{
	for( int p=1; p<6; ++p )
	{
		if (ISREL3D_BACK == planes[p].classifyPointRelation(pos))
			return false;
	}

	return true;
}

inline void frustum::makePlane( const matrix4& mat, VFPLANES side, plane3df& plane )
{
	switch (side)
	{
	case VF_NEAR_PLANE:
		plane.Normal.X = mat[2];
		plane.Normal.Y = mat[6];
		plane.Normal.Z = mat[10];
		plane.D =        mat[14];
		break;
	case VF_FAR_PLANE:
		plane.Normal.X = mat[3 ] - mat[2];
		plane.Normal.Y = mat[7 ] - mat[6];
		plane.Normal.Z = mat[11] - mat[10];
		plane.D =        mat[15] - mat[14];
		break;
	case VF_LEFT_PLANE:
		plane.Normal.X = mat[3 ] + mat[0];
		plane.Normal.Y = mat[7 ] + mat[4];
		plane.Normal.Z = mat[11] + mat[8];
		plane.D =        mat[15] + mat[12];
		break;
	case VF_RIGHT_PLANE:
		plane.Normal.X = mat[3 ] - mat[0];
		plane.Normal.Y = mat[7 ] - mat[4];
		plane.Normal.Z = mat[11] - mat[8];
		plane.D =        mat[15] - mat[12];
		break;
	case VF_TOP_PLANE:
		plane.Normal.X = mat[3 ] - mat[1];
		plane.Normal.Y = mat[7 ] - mat[5];
		plane.Normal.Z = mat[11] - mat[9];
		plane.D =        mat[15] - mat[13];
		break;
	case VF_BOTTOM_PLANE:
		plane.Normal.X = mat[3 ] + mat[1];
		plane.Normal.Y = mat[7 ] + mat[5];
		plane.Normal.Z = mat[11] + mat[9];
		plane.D =        mat[15] + mat[13];
		break;
	default:
		ASSERT(false);
	}

	plane.normalize();
}

inline bool frustum::equals( const frustum& other ) const
{
	for (int i=0; i<VF_PLANE_COUNT; ++i)
	{
		if (planes[i] != other.planes[i])
			return false;
	}
	return true;
}
