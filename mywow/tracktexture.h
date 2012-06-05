#pragma once

#include "base.h"
#include "vector3d.h"
#include "quaternion.h"

//纹理坐标变换动画 time key, value

struct TextureAnimKey
{
	TextureAnimKey() 
		: frame(0), offsetU(0), offsetV(0), scaleU(1.0f), scaleV(1.0f), rotate(0.0f) {}
	u32  frame;
	f32   offsetU;
	f32   offsetV;
	f32	scaleU;
	f32   scaleV;
	f32   rotate;
};

struct STrackTexture
{
	TextureAnimKey Begin, End;

	void getValue(f32 frame, f32& offsetU, f32& offsetV, f32& scaleU, f32& scaleV, f32& rotate);

	void getTextureMatrix(f32 frame, matrix4& mat);
};

inline void STrackTexture::getValue( f32 frame, f32& offsetU, f32& offsetV, f32& scaleU, f32& scaleV, f32& rotate )
{
	if (frame <= Begin.frame)
	{
		offsetU = Begin.offsetU;
		offsetV = Begin.offsetV;
		scaleU = Begin.scaleU;
		scaleV = Begin.scaleV;
		rotate = Begin.rotate;
		return;
	}

	if (frame >= End.frame)
	{
		offsetU = End.offsetU;
		offsetV = End.offsetV;
		scaleU = End.scaleU;
		scaleV = End.scaleV;
		rotate = End.rotate;
		return;
	}

	f32 t  = (f32)(End.frame - frame) / (End.frame - Begin.frame);

	offsetU = t * Begin.offsetU + (1-t) * End.offsetU;
	offsetV = t * Begin.offsetV + (1-t) * End.offsetV;
	scaleU = t * Begin.scaleU + (1-t) * End.scaleU;
	scaleV = t * Begin.scaleV + (1-t) * End.scaleV;
	rotate = t * Begin.rotate + (1-t) * End.rotate;
}

//左手
inline void STrackTexture::getTextureMatrix( f32 frame, matrix4& mat )
{
	mat.makeIdentity();

	f32 offsetU, offsetV, scaleU, scaleV, rotate;

	getValue(frame, offsetU, offsetV, scaleU, scaleV, rotate);

	// texture scaling
	if (scaleU != 0 || scaleV != 0)
	{
		// offset to the center of the texture
		mat.m00 = scaleU;
		mat.m11 = scaleV;

		// skip matrix mult since first matrix update
		mat.m20 = (-0.5f * scaleU) + 0.5f;
		mat.m21 = (-0.5f * scaleV) + 0.5f;
	}

	// texture translation
	if (offsetU != 0 || offsetV != 0)
	{
		matrix4 trans;

		trans.m20 = offsetU;
		trans.m21 = offsetV;

		// multiplt the transform by the translation
		mat *= trans;
	}

	if (rotate != 0.0f)
	{
		matrix4 rot;

		f32 theta = DEGTORAD * rotate;
		f32 cosTheta = cos(theta);
		f32 sinTheta = sin(theta);

		// set the rotation portion of the matrix
		rot.m00 = cosTheta;
		rot.m01 = sinTheta;
		rot.m10 = -sinTheta;
		rot.m11 = cosTheta;

		// offset the center of rotation to the center of the texture
		rot.m20 = 0.5f + (0.5f * (sinTheta - cosTheta));
		rot.m21 = 0.5f + (-0.5f * (sinTheta + cosTheta));

		// multiply the rotation and transformation matrices
		mat *= rot;
	}
}