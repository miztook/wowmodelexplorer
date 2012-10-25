#pragma once

#include "base.h"
#include "core.h"
#include "SColor.h"

//顶点使用多流系统以减少传输带宽
/*
	G:		pos, normal, color0(diffuse), color1(specular)
	T:		texturecoord, tanget, binormal
	A:		blend weight, blend indices
	I:		instancing
*/

struct SGVertex_PC
{
	vector3df	Pos;
	SColor		Color;

	inline void set(const vector3df& p, SColor c) { Pos = p; Color = c; }
};

struct SGVertex_PN
{
	vector3df	Pos;
	vector3df	Normal;

	inline void set(const vector3df& p, const vector3df& n) { Pos = p; Normal = n; }
};

struct SGVertex_PNC
{
	vector3df	Pos;
	vector3df Normal;
	SColor Color;

	inline void set(const vector3df& p, const vector3df& n, SColor c) { Pos = p; Normal = n; Color = c;}
};

struct STVertex_1T
{
	vector2df	TCoords;
};

struct STVertex_2T
{
	vector2df	TCoords0;
	vector2df	TCoords1;

	inline void set(const vector2df& t0, const vector2df& t1) { TCoords0 = t0; TCoords1 = t1; }
};

struct STVertex_TTB
{
	vector2df	TCoords;
	vector3df Tangent;
	vector3df Binormal;

	inline void set(const vector2df& tc, const vector3df& tg, const vector3df& bi) { TCoords = tc; Tangent = tg; Binormal = bi; }
};

struct SAVertex
{
	f32		Weights[4];
	u8		BoneIndices[4];
};

struct SGTVertex_PC1T
{
	vector3df Pos;
	SColor	Color;
	vector2df TCoords;

	inline void set(const vector3df& p, SColor c, const vector2df& t) { Pos = p; Color = c; TCoords = t; }
};

inline u32 getStreamPitchFromType(E_STREAM_TYPE type)
{
	switch (type)
	{
	case EST_G_PC:
		return sizeof(SGVertex_PC);
	case EST_G_PN:
		return sizeof(SGVertex_PN);
	case EST_G_PNC:
		return sizeof(SGVertex_PNC);
	case EST_T_1T:
		return sizeof(STVertex_1T);
	case EST_T_2T:
		return sizeof(STVertex_2T);
	case EST_T_TTB:
		return sizeof(STVertex_TTB);
	case EST_A:
		return sizeof(SAVertex);
	case EST_GT_PC_1T:
		return sizeof(SGTVertex_PC1T);
	default:
		_ASSERT(false);
		return 0;
	}
}

//单流系统使用
inline u32 getVertexPitchFromType(E_VERTEX_TYPE type)
{
	switch(type)
	{
	case EVT_BASICCOLOR:
		return sizeof(SGVertex_PC);
	case EVT_BASICTEX_S:
		return sizeof(SGTVertex_PC1T);
	default:
		_ASSERT(false);
		return 0;
	}
}




