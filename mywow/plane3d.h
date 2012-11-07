#pragma once

enum EIntersectionRelation3D
{
	ISREL3D_FRONT = 0,
	ISREL3D_BACK,
	ISREL3D_PLANAR,
	ISREL3D_SPANNING,
	ISREL3D_CLIPPED,

	ISREL3D_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

template <class T>
class plane3d
{
public:
	//
	plane3d(): Normal(0,1,0) { recalculateD(vector3d<T>(0,0,0)); }
	plane3d(const vector3d<T>& MPoint, const vector3d<T>& Normal) : Normal(Normal) { recalculateD(MPoint); }
	plane3d(T px, T py, T pz, T nx, T ny, T nz) : Normal(nx, ny, nz) { recalculateD(vector3d<T>(px, py, pz)); }
	plane3d(const vector3d<T>& point1, const vector3d<T>& point2, const vector3d<T>& point3) { setPlane(point1, point2, point3); }
	plane3d(const vector3d<T> & normal, const T d) : Normal(normal), D(d) { }

	//
	inline bool operator==(const plane3d<T>& other) const { return (equals_(D, other.D) && Normal==other.Normal); }
	inline bool operator!=(const plane3d<T>& other) const { return !(*this == other); }

	//
	inline void recalculateD(const vector3d<T>& MPoint) { D = - MPoint.dotProduct(Normal); }
	inline vector3d<T> getMemberPoint() const { return Normal * -D; }
	void setPlane(const vector3d<T>& point, const vector3d<T>& nvector)
	{
		Normal = nvector;
		recalculateD(point);
	}

	void setPlane(const vector3d<T>& nvect, T d)
	{
		Normal = nvect;
		D = d;
	}

	void setPlane(const vector3d<T>& point1, const vector3d<T>& point2, const vector3d<T>& point3)
	{
		Normal = (point2 - point1).crossProduct(point3 - point1);
		Normal.normalize();
		recalculateD(point1);
	}

	void normalize()
	{
		T lensq = Normal.getLengthSQ();
		if (equals_(lensq, 1.0f))
			return;

		f32 len ;
		if (equals_(lensq, 0.0f))			//avoid div by 0
		{
			s32 f = 1;
			do
			{
				f *= 100;
				lensq *= f;
			} while(equals_(lensq, 0.0f));
			len = squareroot_((f32)f) * squareroot_(lensq);
		}
		else
		{
			len = squareroot_(lensq);
		}

		Normal = Normal * len;
		D = D * len;
	}

	EIntersectionRelation3D classifyPointRelation(const vector3d<T>& point) const
	{
		const T d = Normal.dotProduct(point) + D;

		if (d < -ROUNDING_ERROR_f32)
			return ISREL3D_BACK;

		if (d > ROUNDING_ERROR_f32)
			return ISREL3D_FRONT;

		return ISREL3D_PLANAR;
	}

	bool getIntersectionWithLine(const vector3d<T>& linePoint,
		const vector3d<T>& lineVect,
		vector3d<T>& outIntersection) const
	{
		T t2 = Normal.dotProduct(lineVect);

		if (t2 == 0)
			return false;

		T t =- (Normal.dotProduct(linePoint) + D) / t2;
		outIntersection = linePoint + (lineVect * t);
		return true;
	}

public:
	vector3d<T> Normal;
	T D;
};

typedef plane3d<f32> plane3df;
typedef plane3d<s32> plane3di;
