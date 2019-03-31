#pragma once

//row major matrix
template <class T>
class CMatrix4
{
public:
	//
	CMatrix4() { makeIdentity(); }
	explicit CMatrix4(bool Identity) { if (Identity) makeIdentity(); else makeZero(); }
	CMatrix4(const CMatrix4& other){ (*this) = other; }

	//
	T& operator()(const int32_t row, const int32_t col){ return M[row * 4 + col]; }
	const T& operator()(const int32_t row, const int32_t col) const { return M[row * 4 + col]; }

	T& operator[](uint32_t index){ return M[index]; }
	const T& operator[](uint32_t index) const{ return M[index]; }

	const T* pointer() const { return M; }
	T* pointer() { return M; }

	CMatrix4<T>& operator=(const CMatrix4<T> &other);

	bool operator==(const CMatrix4<T> &other) const;
	bool operator!=(const CMatrix4<T> &other) const { return !(*this == other); }

	CMatrix4<T> operator+(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator+=(const CMatrix4<T>& other);
	CMatrix4<T> operator-(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator-=(const CMatrix4<T>& other);

	CMatrix4<T> operator*(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator*=(const CMatrix4<T>& other) { return setbyproduct(*this, other); }
	CMatrix4<T> operator*(const T& scalar) const;
	CMatrix4<T>& operator*=(const T& scalar);
	CMatrix4<T>& setbyproduct(const CMatrix4<T>& other_a, const CMatrix4<T>& other_b);

	//
	CMatrix4<T>& makeIdentity()
	{
		M[1] = M[2] = M[3] = M[4] =
			M[6] = M[7] = M[8] = M[9] =
			M[11] = M[12] = M[13] = M[14] = 0;
		M[0] = M[5] = M[10] = M[15] = (T)1;
		return *this;
	}
	CMatrix4<T>& makeZero(){ memset(M, 0, 16 * sizeof(T)); return *this; }
	bool makeInverse();
	bool getInverse(CMatrix4<T>& out) const;
	void transpose();

	bool equals(const CMatrix4<T>& other, const T tolerance = (T)ROUNDING_ERROR_f32) const;

	bool isIdentity() const;
	bool isZero() const;
	//
	void transformVect(vector3df& vect, float& z) const;
	void transformVect(vector3df& vect) const;
	void transformForClipPlane(plane3df& plane) const;
	void transformPlane(plane3df& plane) const;
	void transformBox(aabbox3df& box) const;
	void rotateVect(vector3df& vect) const;
	void inverseRotateVect(vector3df& vect) const;

	CMatrix4<T>& setTranslation(const vector3d<T>& translation);
	vector3d<T> getTranslation() const { return vector3d<T>(M[12], M[13], M[14]); }

	CMatrix4<T>& setRotationRadians(const vector3d<T>& rotation);
	CMatrix4<T>& setRotationDegrees(const vector3d<T>& rotation){ return setRotationRadians(rotation * DEGTORAD); }
	vector3d<T> getRotationRadians() const;
	vector3d<T> getRotationDegrees() const { return getRotationRadians() * RADTODEG; }

	CMatrix4<T>& setScale(const vector3d<T>& scale);
	CMatrix4<T>& setScale(const T scale) { return setScale(vector3d<T>(scale, scale, scale)); }
	vector3d<T> getScale() const;
	// camera view, perspective
	//camera
	CMatrix4<T>& buildCameraLookAtMatrixLH(
		const vector3df& position,
		const vector3df& target,
		const vector3df& upVector);

	CMatrix4<T>& buildCameraLookAtMatrixRH(
		const vector3df& position,
		const vector3df& target,
		const vector3df& upVector);

	//projection
	CMatrix4<T>& buildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	//
	CMatrix4<T>& buildTextureTransform(float rotateRad,
		const vector2df &rotatecenter,
		const vector2df &translate,
		const vector2df &scale);

	CMatrix4<T>& setTextureRotationCenter(float rotateRad);

	CMatrix4<T>&	 setTextureTranslate(float x, float y);

	CMatrix4<T>& setTextureScale(float sx, float sy);

	CMatrix4<T>& setTextureScaleCenter(float sx, float sy);

	CMatrix4<T>& setM(const T* data);

	static const CMatrix4<T>& Identity() { static CMatrix4<T> m(true); return m; }
	static const CMatrix4<T>& Zero() { static CMatrix4<T> m(false); return m; }

	CMatrix4<T>& buildMatrix(const vector3d<T>& vDir, const vector3d<T>& vUp, const vector3d<T>& vPos);

public:
	union
	{
		T M[16];
		struct
		{
			T m00, m01, m02, m03;
			T m10, m11, m12, m13;
			T m20, m21, m22, m23;
			T m30, m31, m32, m33;
		};
	};
};

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildMatrix(const vector3d<T>& vDir, const vector3d<T>& vUp, const vector3d<T>& vPos)
{
	vector3d<T>	vX, vY, vZ;

	vZ = vDir;
	vZ.normalize();
	vY = vUp;
	vY.normalize();
	vX = vY.crossProduct(vZ);
	vX.normalize();

	m00 = vX.X;
	m01 = vX.Y;
	m02 = vX.Z;
	m03 = (T)0.0f;

	m10 = vY.X;
	m11 = vY.Y;
	m12 = vY.Z;
	m13 = (T)0.0f;

	m20 = vZ.X;
	m21 = vZ.Y;
	m22 = vZ.Z;
	m23 = (T)0.0f;

	m30 = vPos.X;
	m31 = vPos.Y;
	m32 = vPos.Z;
	m33 = (T)1.0f;

	return *this;
}

template <class T>
inline void CMatrix4<T>::transpose()
{
	T tmp;
	tmp = m01;
	m01 = m10;
	m10 = tmp;

	tmp = m02;
	m02 = m20;
	m20 = tmp;

	tmp = m03;
	m03 = m30;
	m30 = tmp;

	tmp = m12;
	m12 = m21;
	m21 = tmp;

	tmp = m13;
	m13 = m31;
	m31 = tmp;

	tmp = m23;
	m23 = m32;
	m32 = tmp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setM(const T* data)
{
	memcpy(M, data, 16 * sizeof(T));
	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureScaleCenter(float sx, float sy)
{
	M[0] = (T)sx;
	M[5] = (T)sy;
	M[8] = (T)(0.5f - 0.5f * sx);
	M[9] = (T)(0.5f - 0.5f * sy);

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureScale(float sx, float sy)
{
	M[0] = (T)sx;
	M[5] = (T)sy;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureTranslate(float x, float y)
{
	M[8] = (T)x;
	M[9] = (T)y;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTextureRotationCenter(float rotateRad)
{
	const float c = cosf(rotateRad);
	const float s = sinf(rotateRad);
	M[0] = (T)c;
	M[1] = (T)s;

	M[4] = (T)-s;
	M[5] = (T)c;

	M[8] = (T)(0.5f * (s - c) + 0.5f);
	M[9] = (T)(-0.5f * (s + c) + 0.5f);

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildTextureTransform(float rotateRad, const vector2df &rotatecenter, const vector2df &translate, const vector2df &scale)
{
	const float c = cosf(rotateRad);
	const float s = sinf(rotateRad);

	M[0] = (T)(c * scale.X);
	M[1] = (T)(s * scale.Y);
	M[2] = 0;
	M[3] = 0;

	M[4] = (T)(-s * scale.X);
	M[5] = (T)(c * scale.Y);
	M[6] = 0;
	M[7] = 0;

	M[8] = (T)(c * scale.X * rotatecenter.X + -s * rotatecenter.Y + translate.X);
	M[9] = (T)(s * scale.Y * rotatecenter.X + c * rotatecenter.Y + translate.Y);
	M[10] = 1;
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = 0;
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator=(const CMatrix4<T> &other)
{
	ASSERT(this != &other);
	memcpy(M, other.M, 16 * sizeof(T));
	return *this;
}

template <class T>
inline bool CMatrix4<T>::operator==(const CMatrix4<T>& other) const
{
	if (this == &other) return true;
	return memcmp(this, &other, sizeof(CMatrix4<T>)) == 0;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator+(const CMatrix4<T>& other) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] + other[0];
	temp[1] = M[1] + other[1];
	temp[2] = M[2] + other[2];
	temp[3] = M[3] + other[3];
	temp[4] = M[4] + other[4];
	temp[5] = M[5] + other[5];
	temp[6] = M[6] + other[6];
	temp[7] = M[7] + other[7];
	temp[8] = M[8] + other[8];
	temp[9] = M[9] + other[9];
	temp[10] = M[10] + other[10];
	temp[11] = M[11] + other[11];
	temp[12] = M[12] + other[12];
	temp[13] = M[13] + other[13];
	temp[14] = M[14] + other[14];
	temp[15] = M[15] + other[15];

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator+=(const CMatrix4<T>& other)
{
	M[0] += other[0];
	M[1] += other[1];
	M[2] += other[2];
	M[3] += other[3];
	M[4] += other[4];
	M[5] += other[5];
	M[6] += other[6];
	M[7] += other[7];
	M[8] += other[8];
	M[9] += other[9];
	M[10] += other[10];
	M[11] += other[11];
	M[12] += other[12];
	M[13] += other[13];
	M[14] += other[14];
	M[15] += other[15];

	return *this;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator-(const CMatrix4<T>& other) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] - other[0];
	temp[1] = M[1] - other[1];
	temp[2] = M[2] - other[2];
	temp[3] = M[3] - other[3];
	temp[4] = M[4] - other[4];
	temp[5] = M[5] - other[5];
	temp[6] = M[6] - other[6];
	temp[7] = M[7] - other[7];
	temp[8] = M[8] - other[8];
	temp[9] = M[9] - other[9];
	temp[10] = M[10] - other[10];
	temp[11] = M[11] - other[11];
	temp[12] = M[12] - other[12];
	temp[13] = M[13] - other[13];
	temp[14] = M[14] - other[14];
	temp[15] = M[15] - other[15];

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator-=(const CMatrix4<T>& other)
{
	M[0] -= other[0];
	M[1] -= other[1];
	M[2] -= other[2];
	M[3] -= other[3];
	M[4] -= other[4];
	M[5] -= other[5];
	M[6] -= other[6];
	M[7] -= other[7];
	M[8] -= other[8];
	M[9] -= other[9];
	M[10] -= other[10];
	M[11] -= other[11];
	M[12] -= other[12];
	M[13] -= other[13];
	M[14] -= other[14];
	M[15] -= other[15];

	return *this;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator*(const CMatrix4<T>& other) const
{
	CMatrix4<T> m3;
	const T* m1 = M;
	const T* m2 = other.M;

	m3[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
	m3[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
	m3[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
	m3[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

	m3[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
	m3[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
	m3[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
	m3[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

	m3[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
	m3[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
	m3[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
	m3[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

	m3[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
	m3[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
	m3[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
	m3[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];

	return m3;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator*(const T& scalar) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] * scalar;
	temp[1] = M[1] * scalar;
	temp[2] = M[2] * scalar;
	temp[3] = M[3] * scalar;
	temp[4] = M[4] * scalar;
	temp[5] = M[5] * scalar;
	temp[6] = M[6] * scalar;
	temp[7] = M[7] * scalar;
	temp[8] = M[8] * scalar;
	temp[9] = M[9] * scalar;
	temp[10] = M[10] * scalar;
	temp[11] = M[11] * scalar;
	temp[12] = M[12] * scalar;
	temp[13] = M[13] * scalar;
	temp[14] = M[14] * scalar;
	temp[15] = M[15] * scalar;

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator*=(const T& scalar)
{
	M[0] *= scalar;
	M[1] *= scalar;
	M[2] *= scalar;
	M[3] *= scalar;
	M[4] *= scalar;
	M[5] *= scalar;
	M[6] *= scalar;
	M[7] *= scalar;
	M[8] *= scalar;
	M[9] *= scalar;
	M[10] *= scalar;
	M[11] *= scalar;
	M[12] *= scalar;
	M[13] *= scalar;
	M[14] *= scalar;
	M[15] *= scalar;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setbyproduct(const CMatrix4<T>& other_a, const CMatrix4<T>& other_b)
{
	const T *m1 = other_a.M;
	const T *m2 = other_b.M;

	T tmp[16];

	tmp[0] = m1[0] * m2[0] + m1[1] * m2[4] + m1[2] * m2[8] + m1[3] * m2[12];
	tmp[1] = m1[0] * m2[1] + m1[1] * m2[5] + m1[2] * m2[9] + m1[3] * m2[13];
	tmp[2] = m1[0] * m2[2] + m1[1] * m2[6] + m1[2] * m2[10] + m1[3] * m2[14];
	tmp[3] = m1[0] * m2[3] + m1[1] * m2[7] + m1[2] * m2[11] + m1[3] * m2[15];

	tmp[4] = m1[4] * m2[0] + m1[5] * m2[4] + m1[6] * m2[8] + m1[7] * m2[12];
	tmp[5] = m1[4] * m2[1] + m1[5] * m2[5] + m1[6] * m2[9] + m1[7] * m2[13];
	tmp[6] = m1[4] * m2[2] + m1[5] * m2[6] + m1[6] * m2[10] + m1[7] * m2[14];
	tmp[7] = m1[4] * m2[3] + m1[5] * m2[7] + m1[6] * m2[11] + m1[7] * m2[15];

	tmp[8] = m1[8] * m2[0] + m1[9] * m2[4] + m1[10] * m2[8] + m1[11] * m2[12];
	tmp[9] = m1[8] * m2[1] + m1[9] * m2[5] + m1[10] * m2[9] + m1[11] * m2[13];
	tmp[10] = m1[8] * m2[2] + m1[9] * m2[6] + m1[10] * m2[10] + m1[11] * m2[14];
	tmp[11] = m1[8] * m2[3] + m1[9] * m2[7] + m1[10] * m2[11] + m1[11] * m2[15];

	tmp[12] = m1[12] * m2[0] + m1[13] * m2[4] + m1[14] * m2[8] + m1[15] * m2[12];
	tmp[13] = m1[12] * m2[1] + m1[13] * m2[5] + m1[14] * m2[9] + m1[15] * m2[13];
	tmp[14] = m1[12] * m2[2] + m1[13] * m2[6] + m1[14] * m2[10] + m1[15] * m2[14];
	tmp[15] = m1[12] * m2[3] + m1[13] * m2[7] + m1[14] * m2[11] + m1[15] * m2[15];

	memcpy(M, tmp, sizeof(T)* 16);

	return *this;
}

template <class T>
inline bool CMatrix4<T>::makeInverse()
{
	CMatrix4<T> temp(false);
	if (getInverse(temp))
	{
		*this = temp;
		return true;
	}
	return false;
}

template <class T>
inline bool CMatrix4<T>::getInverse(CMatrix4<T>& out) const
{
	const CMatrix4<T> &m = *this;

	float d = (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) -
		(m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
		(m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)) +
		(m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) -
		(m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
		(m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0));

	if (iszero_(d))
		return false;

	d = reciprocal_(d);

	out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(2, 3) * m(3, 2)) +
		m(1, 2) * (m(2, 3) * m(3, 1) - m(2, 1) * m(3, 3)) +
		m(1, 3) * (m(2, 1) * m(3, 2) - m(2, 2) * m(3, 1)));
	out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(0, 3) * m(3, 2)) +
		m(2, 2) * (m(0, 3) * m(3, 1) - m(0, 1) * m(3, 3)) +
		m(2, 3) * (m(0, 1) * m(3, 2) - m(0, 2) * m(3, 1)));
	out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(0, 3) * m(1, 2)) +
		m(3, 2) * (m(0, 3) * m(1, 1) - m(0, 1) * m(1, 3)) +
		m(3, 3) * (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)));
	out(0, 3) = d * (m(0, 1) * (m(1, 3) * m(2, 2) - m(1, 2) * m(2, 3)) +
		m(0, 2) * (m(1, 1) * m(2, 3) - m(1, 3) * m(2, 1)) +
		m(0, 3) * (m(1, 2) * m(2, 1) - m(1, 1) * m(2, 2)));
	out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(2, 3) * m(3, 0)) +
		m(1, 3) * (m(2, 2) * m(3, 0) - m(2, 0) * m(3, 2)) +
		m(1, 0) * (m(2, 3) * m(3, 2) - m(2, 2) * m(3, 3)));
	out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(0, 3) * m(3, 0)) +
		m(2, 3) * (m(0, 2) * m(3, 0) - m(0, 0) * m(3, 2)) +
		m(2, 0) * (m(0, 3) * m(3, 2) - m(0, 2) * m(3, 3)));
	out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(0, 3) * m(1, 0)) +
		m(3, 3) * (m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)) +
		m(3, 0) * (m(0, 3) * m(1, 2) - m(0, 2) * m(1, 3)));
	out(1, 3) = d * (m(0, 2) * (m(1, 3) * m(2, 0) - m(1, 0) * m(2, 3)) +
		m(0, 3) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
		m(0, 0) * (m(1, 2) * m(2, 3) - m(1, 3) * m(2, 2)));
	out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(2, 1) * m(3, 0)) +
		m(1, 0) * (m(2, 1) * m(3, 3) - m(2, 3) * m(3, 1)) +
		m(1, 1) * (m(2, 3) * m(3, 0) - m(2, 0) * m(3, 3)));
	out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(0, 1) * m(3, 0)) +
		m(2, 0) * (m(0, 1) * m(3, 3) - m(0, 3) * m(3, 1)) +
		m(2, 1) * (m(0, 3) * m(3, 0) - m(0, 0) * m(3, 3)));
	out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)) +
		m(3, 0) * (m(0, 1) * m(1, 3) - m(0, 3) * m(1, 1)) +
		m(3, 1) * (m(0, 3) * m(1, 0) - m(0, 0) * m(1, 3)));
	out(2, 3) = d * (m(0, 3) * (m(1, 1) * m(2, 0) - m(1, 0) * m(2, 1)) +
		m(0, 0) * (m(1, 3) * m(2, 1) - m(1, 1) * m(2, 3)) +
		m(0, 1) * (m(1, 0) * m(2, 3) - m(1, 3) * m(2, 0)));
	out(3, 0) = d * (m(1, 0) * (m(2, 2) * m(3, 1) - m(2, 1) * m(3, 2)) +
		m(1, 1) * (m(2, 0) * m(3, 2) - m(2, 2) * m(3, 0)) +
		m(1, 2) * (m(2, 1) * m(3, 0) - m(2, 0) * m(3, 1)));
	out(3, 1) = d * (m(2, 0) * (m(0, 2) * m(3, 1) - m(0, 1) * m(3, 2)) +
		m(2, 1) * (m(0, 0) * m(3, 2) - m(0, 2) * m(3, 0)) +
		m(2, 2) * (m(0, 1) * m(3, 0) - m(0, 0) * m(3, 1)));
	out(3, 2) = d * (m(3, 0) * (m(0, 2) * m(1, 1) - m(0, 1) * m(1, 2)) +
		m(3, 1) * (m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0)) +
		m(3, 2) * (m(0, 1) * m(1, 0) - m(0, 0) * m(1, 1)));
	out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) +
		m(0, 1) * (m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2)) +
		m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0)));

	return true;
}

template <class T>
inline bool CMatrix4<T>::equals(const CMatrix4<T>& other, const T tolerance) const
{
	for (int32_t i = 0; i < 16; ++i)
	if (!equals_(M[i], other.M[i], tolerance))
		return false;

	return true;
}

template <class T>
inline bool CMatrix4<T>::isIdentity() const
{
	return (*this) == Identity();
}

template <class T>
bool CMatrix4<T>::isZero() const
{
	return (*this) == Zero();
}

template <class T>
inline void CMatrix4<T>::transformVect(vector3df& vect, float& z) const
{
	float vector[3];

	vector[0] = vect.X*M[0] + vect.Y*M[4] + vect.Z*M[8] + M[12];
	vector[1] = vect.X*M[1] + vect.Y*M[5] + vect.Z*M[9] + M[13];
	vector[2] = vect.X*M[2] + vect.Y*M[6] + vect.Z*M[10] + M[14];
	z = vect.X*M[3] + vect.Y*M[7] + vect.Z*M[11] + M[15];

	vect.X = vector[0];
	vect.Y = vector[1];
	vect.Z = vector[2];
}

template <class T>
inline void CMatrix4<T>::transformVect(vector3df& vect) const
{
	float vector[3];

	vector[0] = vect.X*M[0] + vect.Y*M[4] + vect.Z*M[8] + M[12];
	vector[1] = vect.X*M[1] + vect.Y*M[5] + vect.Z*M[9] + M[13];
	vector[2] = vect.X*M[2] + vect.Y*M[6] + vect.Z*M[10] + M[14];

	vect.X = vector[0];
	vect.Y = vector[1];
	vect.Z = vector[2];
}

template <class T>
inline void CMatrix4<T>::transformPlane(plane3df& plane) const
{
	vector3df point = plane.getMemberPoint();
	transformVect(point);

	CMatrix4<T> m = *this;
	m.makeInverse();
	m.transpose();

	vector3df normal = plane.Normal;
	m.transformVect(normal);

	plane.setPlane(point, normal);
}

template <class T>
inline void CMatrix4<T>::transformForClipPlane(plane3df& plane) const
{
	CMatrix4<T> m = *this;
	m.makeInverse();
	m.transpose();

	plane.normalize();

	float x = plane.Normal.X;
	float y = plane.Normal.Y;
	float z = plane.Normal.Z;
	float d = plane.D;

	vector3df normal;
	float D;

	normal.X = x * m.M[0] + y * m.M[4] + z * m.M[8] + d * m.M[12];
	normal.Y = x * m.M[1] + y * m.M[5] + z * m.M[9] + d * m.M[13];
	normal.Z = x * m.M[2] + y * m.M[6] + z * m.M[10] + d * m.M[14];
	D = x * m.M[3] + y * m.M[7] + z * m.M[11] + d * m.M[15];

	plane.setPlane(normal, D);
}

template <class T>
inline void CMatrix4<T>::transformBox(aabbox3df& box) const
{
	aabbox3df BoundingBox;
	vector3df points[8];
	box.makePoints(points);

	vector3df point = points[0];
	transformVect(point);
	BoundingBox.reset(point);
	for (uint32_t i = 1; i < 8; ++i)
	{
		point = points[i];
		transformVect(point);
		BoundingBox.addInternalPoint(point);
	}

	box = BoundingBox;
}

template <class T>
inline void CMatrix4<T>::rotateVect(vector3df& vect) const
{
	vector3df tmp = vect;
	vect.X = tmp.X*M[0] + tmp.Y*M[4] + tmp.Z*M[8];
	vect.Y = tmp.X*M[1] + tmp.Y*M[5] + tmp.Z*M[9];
	vect.Z = tmp.X*M[2] + tmp.Y*M[6] + tmp.Z*M[10];
}

template <class T>
inline  void CMatrix4<T>::inverseRotateVect(vector3df& vect) const
{
	vector3df tmp = vect;
	vect.X = tmp.X*M[0] + tmp.Y*M[1] + tmp.Z*M[2];
	vect.Y = tmp.X*M[4] + tmp.Y*M[5] + tmp.Z*M[6];
	vect.Z = tmp.X*M[8] + tmp.Y*M[9] + tmp.Z*M[10];
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTranslation(const vector3d<T>& translation)
{
	M[12] = translation.X;
	M[13] = translation.Y;
	M[14] = translation.Z;
	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setRotationRadians(const vector3d<T>& rotation)
{
	const float cr = cos(rotation.X);
	const float sr = sin(rotation.X);
	const float cp = cos(rotation.Y);
	const float sp = sin(rotation.Y);
	const float cy = cos(rotation.Z);
	const float sy = sin(rotation.Z);

	M[0] = (T)(cp*cy);
	M[1] = (T)(cp*sy);
	M[2] = (T)(-sp);

	const float srsp = sr*sp;
	const float crsp = cr*sp;

	M[4] = (T)(srsp*cy - cr*sy);
	M[5] = (T)(srsp*sy + cr*cy);
	M[6] = (T)(sr*cp);

	M[8] = (T)(crsp*cy + sr*sy);
	M[9] = (T)(crsp*sy - sr*cy);
	M[10] = (T)(cr*cp);

	return *this;
}

template <class T>
inline vector3d<T> CMatrix4<T>::getRotationRadians() const
{
	const CMatrix4<T> &mat = *this;
	const vector3d<T> scale = getScale();
	const vector3d<float> invScale(reciprocal_(scale.X), reciprocal_(scale.Y), reciprocal_(scale.Z));

	float Y = -asin(mat[2] * invScale.X);
	const float C = cos(Y);

	float rotx, roty, X, Z;

	if (!iszero_(C))
	{
		const float invC = reciprocal_(C);
		rotx = mat[10] * invC * invScale.Z;
		roty = mat[6] * invC * invScale.Y;
		X = atan2(roty, rotx);
		rotx = mat[0] * invC * invScale.X;
		roty = mat[1] * invC * invScale.X;
		Z = atan2(roty, rotx);
	}
	else
	{
		X = 0.0;
		rotx = mat[5] * invScale.Y;
		roty = -mat[4] * invScale.Y;
		Z = atan2(roty, rotx);
	}

	// fix values that get below zero
	// before it would set (!) values to 360
	// that were above 360:
	if (X < 0.0) X += 2 * PI;
	if (Y < 0.0) Y += 2 * PI;
	if (Z < 0.0) Z += 2 * PI;

	return vector3d<T>((T)X, (T)Y, (T)Z);
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setScale(const vector3d<T>& scale)
{
	T x = abs_(scale.X);			// 0
	T y = abs_(scale.Y);			// 5
	T z = abs_(scale.Z);			// 10

	M[0] *= x;
	M[1] *= y;
	M[2] *= z;

	M[4] *= x;
	M[5] *= y;
	M[6] *= z;

	M[8] *= x;
	M[9] *= y;
	M[10] *= z;

	M[12] *= x;
	M[13] *= y;
	M[14] *= z;

	return (*this);
}

template <class T>
inline vector3d<T> CMatrix4<T>::getScale() const
{
	// We have to do the full calculation.
	return vector3d<T>(squareroot_(M[0] * M[0] + M[1] * M[1] + M[2] * M[2]),
		squareroot_(M[4] * M[4] + M[5] * M[5] + M[6] * M[6]),
		squareroot_(M[8] * M[8] + M[9] * M[9] + M[10] * M[10]));
}

template <class T>
inline  CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixLH(
	const vector3df& position,
	const vector3df& target,
	const vector3df& upVector)
{
	vector3df zaxis = target - position;
	zaxis.normalize();

	vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	vector3df yaxis = zaxis.crossProduct(xaxis);

	M[0] = (T)xaxis.X;
	M[1] = (T)yaxis.X;
	M[2] = (T)zaxis.X;
	M[3] = 0;

	M[4] = (T)xaxis.Y;
	M[5] = (T)yaxis.Y;
	M[6] = (T)zaxis.Y;
	M[7] = 0;

	M[8] = (T)xaxis.Z;
	M[9] = (T)yaxis.Z;
	M[10] = (T)zaxis.Z;
	M[11] = 0;

	M[12] = (T)-xaxis.dotProduct(position);
	M[13] = (T)-yaxis.dotProduct(position);
	M[14] = (T)-zaxis.dotProduct(position);
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixRH(
	const vector3df& position,
	const vector3df& target,
	const vector3df& upVector)
{
	vector3df zaxis = position - target;
	zaxis.normalize();

	vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	vector3df yaxis = zaxis.crossProduct(xaxis);

	M[0] = (T)xaxis.X;
	M[1] = (T)yaxis.X;
	M[2] = (T)zaxis.X;
	M[3] = 0;

	M[4] = (T)xaxis.Y;
	M[5] = (T)yaxis.Y;
	M[6] = (T)zaxis.Y;
	M[7] = 0;

	M[8] = (T)xaxis.Z;
	M[9] = (T)yaxis.Z;
	M[10] = (T)zaxis.Z;
	M[11] = 0;

	M[12] = (T)-xaxis.dotProduct(position);
	M[13] = (T)-yaxis.dotProduct(position);
	M[14] = (T)-zaxis.dotProduct(position);
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovLH(
	float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5f));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const float w = (h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zFar - zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(-zNear*zFar / (zFar - zNear));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovRH(
	float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_(tan(fieldOfViewRadians*0.5f));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = h / aspectRatio;

	ASSERT(zNear != zFar); //divide by zero
	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zNear - zFar)); // DirectX version
	//		M[10] = (T)(zFar+zNear/(zNear-zFar)); // OpenGL version
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear*zFar / (zNear - zFar)); // DirectX version
	//		M[14] = (T)(2.0f*zNear*zFar/(zNear-zFar)); // OpenGL version
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveLH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 * zNear / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 * zNear / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zFar - zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear*zFar / (zNear - zFar));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveRH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 * zNear / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 * zNear / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zNear - zFar));
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear*zFar / (zNear - zFar));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = (T)(h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	ASSERT(zNear != 0.0f);  //divide by zero

	M[0] = w / zNear;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h / zNear;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(zFar / (zFar - zNear));
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(-zNear*zFar / (zFar - zNear));
	M[15] = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = (T)(h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	ASSERT(zNear != 0.0f);  //divide by zero

	M[0] = w / zNear;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h / zNear;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(1 / (zNear - zFar));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear / (zNear - zFar));
	M[15] = -1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoLH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(1 / (zFar - zNear));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear / (zNear - zFar));
	M[15] = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoRH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	M[0] = (T)(2 / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(1 / (zNear - zFar));
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear / (zNear - zFar));
	M[15] = -1;

	return *this;
}

typedef CMatrix4<float> matrix4;
