#pragma once

#include "base.h"

#include "function.h"
#include "dimension2d.h"
#include "vector2d.h"
#include "vector3d.h"
#include "rect.h"
#include "linklist.h"
#include "triangle3d.h"
#include "line2d.h"
#include "line3d.h"
#include "plane3d.h"
#include "aabbox3d.h"
#include "obbox.h"
#include "matrix4.h"
#include "frustum.h"
#include "quaternion.h"
#include "algorithm.h"
#include "animation.h"
#include "fixstring.h"
#include "tracktexture.h"
#include "encoding.h"
#include "strutil.h"

struct  SDrawParam
{
	uint16_t		voffset0;
	uint16_t		voffset1;
	int32_t		baseVertIndex;
	uint32_t		minVertIndex;
	uint32_t		numVertices;
	uint32_t		startIndex;
};

struct S2DBlendParam
{	
	bool alpha;
	bool alphaChannel;
	E_BLEND_FACTOR srcBlend;
	E_BLEND_FACTOR destBlend;
	
	int alphaBlendEnable() const
	{
		return alpha || alphaChannel ? 1 : 0;
	}

	S2DBlendParam()
		: alpha(false), alphaChannel(false), srcBlend(EBF_ONE), destBlend(EBF_ZERO)
	{
	}

	S2DBlendParam(bool _alpha, bool _alphaChannel, E_BLEND_FACTOR _srcBlend, E_BLEND_FACTOR _destBlend)
		: alpha(_alpha), alphaChannel(_alphaChannel), srcBlend(_srcBlend), destBlend(_destBlend)
	{
	}

	bool operator!=(const S2DBlendParam& b) const
	{
		return alpha != b.alpha ||
			alphaChannel != b.alphaChannel ||
			srcBlend != b.srcBlend ||
			destBlend != b.destBlend;
	}

	bool operator==(const S2DBlendParam& b) const
	{
		return alpha == b.alpha &&
			alphaChannel == b.alphaChannel &&
			srcBlend == b.srcBlend &&
			destBlend == b.destBlend;
	}

	bool operator<(const S2DBlendParam& c) const
	{
		int a = alphaBlendEnable();
		int b = c.alphaBlendEnable();
		if (a != b)
			return a < b;
		else if (srcBlend != c.srcBlend)
			return srcBlend < c.srcBlend;
		else if (destBlend != c.destBlend)
			return destBlend < c.destBlend;
		else
			return false;
	}

	static const S2DBlendParam& OpaqueSource()
	{
		static S2DBlendParam m(false, false, EBF_ONE, EBF_ZERO);
		return m;
	}

	static const S2DBlendParam& UIFontBlendParam()
	{
		static S2DBlendParam m(false, true, EBF_ONE, EBF_ONE_MINUS_SRC_ALPHA);
		return m;
	}

	static const S2DBlendParam& UITextureBlendParam()
	{
		static S2DBlendParam m(false, true, EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA);
		return m;
	}
};

struct SBoneMatrixArray
{
	SBoneMatrixArray(matrix4* mats, uint8_t size, uint16_t weights) : matrices(mats), count(size)
	{
		maxWeights = clamp_(weights, (uint16_t)1, (uint16_t)4);
	}
	matrix4*		matrices;
	uint8_t		count;
	uint16_t		maxWeights;
};