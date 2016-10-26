#pragma once

#include "core.h"
#include "SColor.h"

struct SVertex_P
{
	vector3df Pos;

	 void set(const vector3df& p) { Pos = p; }
};

struct SVertex_PC
{
	vector3df	Pos;
	SColor		Color;

	void set(const vector3df& p, SColor c) { Pos = p; Color = c; }
};

struct SVertex_PCT
{
	vector3df Pos;
	SColor	Color;
	vector2df TCoords;

	void set(const vector3df& p, SColor c, const vector2df& t) { Pos = p; Color = c; TCoords = t; }
};

struct SVertex_PN
{
	vector3df	Pos;
	vector3df	Normal;

	void set(const vector3df& p, const vector3df& n) { Pos = p; Normal = n; }
};

struct SVertex_PNC
{
	vector3df	Pos;
	vector3df Normal;
	SColor Color;

	 void set(const vector3df& p, const vector3df& n, SColor c) { Pos = p; Normal = n; Color = c;}
};

struct SVertex_PNT
{
	vector3df Pos;
	vector3df Normal;
	vector2df TCoords;

	 void set(const vector3df& p, const vector3df& n, const vector2df& t) { Pos = p; Normal = n; TCoords = t; }
};

struct SVertex_PT 
{
	vector3df Pos;
	vector2df TCoords;

	void set(const vector3df& p, const vector2df& t) { Pos = p; TCoords = t; }
};

struct SVertex_PNCT
{
	vector3df	Pos;
	vector3df Normal;
	SColor Color;
	vector2df TCoords;

	 void set(const vector3df& p, const vector3df& n, SColor c, const vector2df& t) { Pos = p; Normal = n; Color = c; TCoords = t; }
};

struct SVertex_PNCT2
{
	vector3df	Pos;
	vector3df Normal;
	SColor Color;
	vector2df TCoords0;
	vector2df TCoords1;

	void set(const vector3df& p, const vector3df& n, SColor c, const vector2df& t0, const vector2df& t1) { Pos = p; Normal = n; Color = c; TCoords0 = t0; TCoords1 = t1; }
};

// struct SVertex_PNTW
// {
// 	vector3df Pos;
// 	vector3df Normal;
// 	vector2df TCoords;
// 	f32		Weights[4];
// 
// 	 void set(const vector3df& p, const vector3df& n, const vector2df& t) { Pos = p; Normal = n; TCoords = t; }
// };

struct SVertex_PNT2W
{
	vector3df Pos;
	vector3df Normal;
	vector2df TCoords0;
	vector2df TCoords1;
	u8		Weights[4];

	 void set(const vector3df& p, const vector3df& n, const vector2df& t, const vector2df& t1) 
	{ Pos = p; Normal = n; TCoords0 = t; TCoords1 = t1; }
};

struct SVertex_A
{
	u8		BoneIndices[4];
};


inline u32 getStreamPitchFromType(E_STREAM_TYPE type)
{
	switch (type)
	{
	case EST_P:
		return sizeof(SVertex_P);
	case EST_PC:
		return sizeof(SVertex_PC);
	case EST_PCT:
		return sizeof(SVertex_PCT);
	case EST_PN:
		return sizeof(SVertex_PN);
	case EST_PNC:
		return sizeof(SVertex_PNC);
	case EST_PNT:
		return sizeof(SVertex_PNT);
	case EST_PT:
		return sizeof(SVertex_PT);
	case EST_PNCT:
		return sizeof(SVertex_PNCT);
	case EST_PNCT2:
		return sizeof(SVertex_PNCT2);
	case EST_PNT2W:
		return sizeof(SVertex_PNT2W);
	case EST_A:
		return sizeof(SVertex_A);

	default:
		ASSERT(false);
		return 0;
	}
}

inline bool vertexHasColor(E_STREAM_TYPE type)
{
	return type == EST_PC || type == EST_PCT || type == EST_PNC || type == EST_PNCT || type == EST_PNCT2;
}

inline void deleteVerticesFromType(E_VERTEX_TYPE type, void* vertices)
{
	switch (type)
	{
	case EVT_P:
		DELETE_ARRAY(SVertex_P, vertices);
		break;
	case EVT_PC:
		DELETE_ARRAY(SVertex_PC, vertices);
		break;
	case EVT_PCT:
		DELETE_ARRAY(SVertex_PCT, vertices);
		break;
	case EVT_PN:
		DELETE_ARRAY(SVertex_PN, vertices);
		break;
	case EVT_PNC:
		DELETE_ARRAY(SVertex_PNC, vertices);
		break;
	case EVT_PNT:
		DELETE_ARRAY(SVertex_PNT, vertices);
		break;
	case EVT_PT:
		DELETE_ARRAY(SVertex_PT, vertices);
		break;
	case EVT_PNCT:
		DELETE_ARRAY(SVertex_PNCT, vertices);
		break;
	case EVT_PNCT2:
		DELETE_ARRAY(SVertex_PNCT2, vertices);
		break;
	case EST_PNT2W:
		DELETE_ARRAY(SVertex_PNT2W, vertices);
		break;
	case EST_A:
		DELETE_ARRAY(SVertex_A, vertices);
		break;
	default:
		ASSERT(false);
		break;
	}
}

inline void deleteIndicesFromType(E_INDEX_TYPE type, void* indices)
{
	switch (type)
	{
	case EIT_16BIT:
		DELETE_ARRAY(u16, indices);
		break;
	case EIT_32BIT:
		DELETE_ARRAY(u32, indices);
		break;
	default:
		ASSERT(false);
		break;
	}
}





