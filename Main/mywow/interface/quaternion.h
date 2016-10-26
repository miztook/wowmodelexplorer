#pragma once

class quaternion
{
public:
	//
	 quaternion() : X(0), Y(0), Z(0), W(0) {}
	 quaternion(f32 x, f32 y, f32 z, f32 w) : X(x), Y(y), Z(z), W(w) { }
	 quaternion(f32 pitch, f32 yaw, f32 roll) { fromEuler( pitch, yaw, roll ); }
	 explicit quaternion(const vector3df& vec) { fromEuler( vec ); }
	 quaternion(f32 angle, const vector3df& axis) { fromAngleAxis( angle, axis ); }
	 quaternion(const quaternion& other) { *this = other; }
	//
	quaternion& operator=(const quaternion& other)
	{
		ASSERT(this != &other);
		X = other.X;
		Y = other.Y;
		Z = other.Z;
		W = other.W;
		return *this;
	}

	bool equals(const quaternion& other, const f32 tolerance=(f32)ROUNDING_ERROR_f32) const { return equals_(X, other.X, tolerance) && equals_(Y, other.Y, tolerance) && equals_(Z, other.Z, tolerance) && equals_(W, other.W, tolerance);}
	bool operator==(const quaternion& other) const 
	{
		if ( this == &other ) return true;
		return memcmp(this, &other, sizeof(quaternion)) == 0;
	}
	 bool operator!=(const quaternion& other) const { return !(*this == other ); }
	 quaternion operator+(const quaternion& other) const { return quaternion(X+other.X, Y+other.Y, Z+other.Z, W+other.W); }
	 quaternion operator*(const quaternion& other) const;
	 quaternion operator*(f32 s) const { return quaternion(s*X, s*Y, s*Z, s*W); }
	 quaternion& operator*=(f32 s) { X*=s; Y*=s; Z*=s; W*=s; return *this; }
	 vector3df operator*(const vector3df& v) const;
	 matrix4 operator*(const matrix4& v) const;
	 quaternion& operator*=(const quaternion& other) { return (*this = other * (*this)); }

	//
	 quaternion& set(f32 x, f32 y, f32 z, f32 w) { X = x; Y = y; Z = z; W = w; return *this; }
	 f32 dotProduct(const quaternion& other) const { return (X * other.X) + (Y * other.Y) + (Z * other.Z) + (W * other.W); }
	quaternion& fromAngleAxis (f32 angle, const vector3df& axis);
	void toAngleAxis (f32 &angle, vector3df& axis) const;
	quaternion& fromEuler( f32 pitch, f32 yaw, f32 roll );
	 quaternion& fromEuler( const vector3df& vec ) { return fromEuler(vec.X, vec.Y, vec.Z); }
	void toEuler(vector3df& euler) const;
	 quaternion& makeIdentity()	{ W = 1.f; X = 0.f; Y = 0.f; Z = 0.f; return *this; }
	quaternion& normalize();
	quaternion& fromMatrix(const matrix4& m);
	void getMatrix( matrix4& dest ) const;				//
	static quaternion slerp( quaternion q1, quaternion q2, f32 interpolate );
	quaternion& rotationFromTo(const vector3df& from, const vector3df& to, const vector3df& axisOpposite);
	void transformVect( vector3df& vect) const;

public:
	f32 X;		//imaginary
	f32 Y;
	f32 Z;
	f32 W;		//real
};

inline quaternion quaternion::operator*(const quaternion& other) const
{
	quaternion tmp;

	tmp.W = (other.W * W) - (other.X * X) - (other.Y * Y) - (other.Z * Z);
	tmp.X = (other.W * X) + (other.X * W) + (other.Y * Z) - (other.Z * Y);
	tmp.Y = (other.W * Y) + (other.Y * W) + (other.Z * X) - (other.X * Z);
	tmp.Z = (other.W * Z) + (other.Z * W) + (other.X * Y) - (other.Y * X);

	return tmp;
}

inline vector3df quaternion::operator*(const vector3df& v) const
{
	vector3df uv, uuv;
	vector3df qvec(X, Y, Z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * W);
	uuv *= 2.0f;

	return v + uv + uuv;
}

inline matrix4 quaternion::operator*( const matrix4& v ) const
{
	matrix4 m;
	getMatrix(m);
	return m * v;
}

inline quaternion& quaternion::fromAngleAxis (f32 angle, const vector3df& axis)
{
	const f32 fHalfAngle = 0.5f*angle;
	const f32 fSin = sinf(fHalfAngle);
	W = cosf(fHalfAngle);
	X = fSin*axis.X;
	Y = fSin*axis.Y;
	Z = fSin*axis.Z;
	return *this;
}

inline void quaternion::toAngleAxis (f32 &angle, vector3df& axis) const
{
	const f32 scale = sqrtf(X*X + Y*Y + Z*Z);

	if (iszero_(scale) || W > 1.0f || W < -1.0f)
	{
		angle = 0.0f;
		axis.X = 0.0f;
		axis.Y = 1.0f;
		axis.Z = 0.0f;
	}
	else
	{
		const f32 invscale = reciprocal_(scale);
		angle = 2.0f * acosf(W);
		axis.X = X * invscale;
		axis.Y = Y * invscale;
		axis.Z = Z * invscale;
	}
}

inline quaternion& quaternion::fromEuler( f32 pitch, f32 yaw, f32 roll )
{
	f32 angle;

	angle = pitch * 0.5f;
	const f32 sr = sin(angle);
	const f32 cr = cos(angle);

	angle = yaw * 0.5f;
	const f32 sp = sin(angle);
	const f32 cp = cos(angle);

	angle = roll * 0.5f;
	const f32 sy = sin(angle);
	const f32 cy = cos(angle);

	const f32 cpcy = cp * cy;
	const f32 spcy = sp * cy;
	const f32 cpsy = cp * sy;
	const f32 spsy = sp * sy;

	X = (f32)(sr * cpcy - cr * spsy);
	Y = (f32)(cr * spcy + sr * cpsy);
	Z = (f32)(cr * cpsy - sr * spcy);
	W = (f32)(cr * cpcy + sr * spsy);

	return normalize();
}


inline void quaternion::toEuler( vector3df& euler ) const
{
	const f32 sqw = W*W;
	const f32 sqx = X*X;
	const f32 sqy = Y*Y;
	const f32 sqz = Z*Z;

	// heading = rotation about z-axis
	euler.Z = (f32) (atan2(2.0f * (X*Y +Z*W),(sqx - sqy - sqz + sqw)));

	// bank = rotation about x-axis
	euler.X = (f32) (atan2(2.0f * (Y*Z +X*W),(-sqx - sqy + sqz + sqw)));

	// attitude = rotation about y-axis
	euler.Y = asinf( clamp_(-2.0f * (X*Z - Y*W), -1.0f, 1.0f) );
}


inline quaternion& quaternion::normalize()
{
	const f32 n = reciprocal_squareroot_( X*X + Y*Y + Z*Z + W*W );

	if (n == 1)
		return *this;

	X *= n;
	Y *= n;
	Z *= n;
	W *= n;

	return *this;
}

inline void quaternion::getMatrix( matrix4& dest ) const
{
	f32 * m = dest.pointer();

	m[0] = 1.0f - 2.0f*Y*Y - 2.0f*Z*Z;
	m[1] = 2.0f*X*Y + 2.0f*Z*W;
	m[2] = 2.0f*X*Z - 2.0f*Y*W;
	m[3] = 0.0f;

	m[4] = 2.0f*X*Y - 2.0f*Z*W;
	m[5] = 1.0f - 2.0f*X*X - 2.0f*Z*Z;
	m[6] = 2.0f*Z*Y + 2.0f*X*W;
	m[7] = 0.0f;

	m[8] = 2.0f*X*Z + 2.0f*Y*W;
	m[9] = 2.0f*Z*Y - 2.0f*X*W;
	m[10] = 1.0f - 2.0f*X*X - 2.0f*Y*Y;
	m[11] = 0.0f;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1.f;
}

inline quaternion quaternion::slerp(quaternion q1, quaternion q2, f32 time)
{
	f32 angle = q1.dotProduct(q2);

	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	f32 scale;
	f32 invscale;

	if ((angle + 1.0f) > 0.05f)
	{
		if ((1.0f - angle) >= 0.05f) // spherical interpolation
		{
			const f32 theta = acosf(angle);
			const f32 invsintheta = reciprocal_(sinf(theta));
			scale = sinf(theta * (1.0f-time)) * invsintheta;
			invscale = sinf(theta * time) * invsintheta;
		}
		else // linear interploation
		{
			scale = 1.0f - time;
			invscale = time;
		}
	}
	else
	{
		q2.set(-q1.Y, q1.X, -q1.W, q1.Z);
		scale = sinf(PI * (0.5f - time));
		invscale = sinf(PI * time);
	}

	return ((q1*scale) + (q2*invscale));
}

inline quaternion& quaternion::fromMatrix( const matrix4& m )
{
	const f32 diag = m(0,0) + m(1,1) + m(2,2) + 1;

	if( diag > 0.0f )
	{
		const f32 scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		X = ( m(2,1) - m(1,2)) / scale;
		Y = ( m(0,2) - m(2,0)) / scale;
		Z = ( m(1,0) - m(0,1)) / scale;
		W = 0.25f * scale;
	}
	else
	{
		if ( m(0,0) > m(1,1) && m(0,0) > m(2,2))
		{
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			const f32 scale = sqrtf( 1.0f + m(0,0) - m(1,1) - m(2,2)) * 2.0f;

			X = 0.25f * scale;
			Y = (m(0,1) + m(1,0)) / scale;
			Z = (m(2,0) + m(0,2)) / scale;
			W = (m(2,1) - m(1,2)) / scale;
		}
		else if ( m(1,1) > m(2,2))
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			const f32 scale = sqrtf( 1.0f + m(1,1) - m(0,0) - m(2,2)) * 2.0f;

			X = (m(0,1) + m(1,0) ) / scale;
			Y = 0.25f * scale;
			Z = (m(1,2) + m(2,1) ) / scale;
			W = (m(0,2) - m(2,0) ) / scale;
		}
		else
		{
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			const f32 scale = sqrtf( 1.0f + m(2,2) - m(0,0) - m(1,1)) * 2.0f;

			X = (m(0,2) + m(2,0)) / scale;
			Y = (m(1,2) + m(2,1)) / scale;
			Z = 0.25f * scale;
			W = (m(1,0) - m(0,1)) / scale;
		}
	}

	return normalize();
}

inline quaternion& quaternion::rotationFromTo( const vector3df& from, const vector3df& to, const vector3df& axisOpposite )
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	vector3df v0 = from;
	vector3df v1 = to;
	v0.normalize();
	v1.normalize();

	const f32 d = v0.dotProduct(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return makeIdentity();
	}
	else if (d <= -1.0f) // exactly opposite
	{
		vector3df axis = axisOpposite;
		return fromAngleAxis(PI, axis);
	}

	const f32 s = squareroot_( (1+d)*2 ); // optimize inv_sqrt
	const f32 invs = 1.f / s;
	const vector3df c = v0.crossProduct(v1)*invs;
	X = c.X;
	Y = c.Y;
	Z = c.Z;
	W = s * 0.5f;

	return *this;
}

inline void quaternion::transformVect( vector3df& vect ) const
{
	f32 m[16];
	m[0] = 1.0f - 2.0f*Y*Y - 2.0f*Z*Z;
	m[1] = 2.0f*X*Y + 2.0f*Z*W;
	m[2] = 2.0f*X*Z - 2.0f*Y*W;
	m[3] = 0.0f;

	m[4] = 2.0f*X*Y - 2.0f*Z*W;
	m[5] = 1.0f - 2.0f*X*X - 2.0f*Z*Z;
	m[6] = 2.0f*Z*Y + 2.0f*X*W;
	m[7] = 0.0f;

	m[8] = 2.0f*X*Z + 2.0f*Y*W;
	m[9] = 2.0f*Z*Y - 2.0f*X*W;
	m[10] = 1.0f - 2.0f*X*X - 2.0f*Y*Y;
	m[11] = 0.0f;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1.f;

	f32 vector[3];

	vector[0] = vect.X*m[0] + vect.Y*m[4] + vect.Z*m[8] + m[12];
	vector[1] = vect.X*m[1] + vect.Y*m[5] + vect.Z*m[9] + m[13];
	vector[2] = vect.X*m[2] + vect.Y*m[6] + vect.Z*m[10] + m[14];

	vect.X = vector[0];
	vect.Y = vector[1];
	vect.Z = vector[2];
}