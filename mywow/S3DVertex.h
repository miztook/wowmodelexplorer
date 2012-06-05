#pragma once

#include "base.h"
#include "core.h"
#include "SColor.h"

struct S3DVertexBasicColor
{
	vector3df Pos;
	SColor	Color;

	void set( const vector3df& pos, SColor color )
	{
		Pos = pos;
		Color = color;
	}

	E_VERTEX_TYPE getType() const { return EVT_BASICCOLOR;  }
};

struct S3DVertexBasicTex
{
	vector3df Pos;
	SColor	Color;
	vector2df TCoords;

	void set(const vector3df& pos, SColor color, const vector2df& tcoords)
	{
		Pos = pos;
		Color = color;
		TCoords = tcoords;
	}

	E_VERTEX_TYPE getType() const { return EVT_BASICTEX; }
};


struct S3DVertex
{
	void set( const vector3df& pos, const vector3df& normal,
		SColor color, const vector2df& tcoords )
	{
		Pos = pos;
		Normal = normal;
		Color = color;
		TCoords = tcoords;
	}

	vector3df Pos;
	vector3df Normal;
	SColor Color;
	vector2df TCoords;

	E_VERTEX_TYPE getType() const { return EVT_STANDARD; }
};

struct S3DVertexModel
{
	void set( const vector3df& pos, const vector3df& normal, const vector2df& tcoords )
	{
		Pos = pos;
		Normal = normal;
		TCoords = tcoords;
	}

	vector3df Pos;
	vector3df Normal;
	vector2df TCoords;
	f32		Weights[4];

	E_VERTEX_TYPE getType() const { return EVT_MODEL; }
};

struct S3DVertexBone
{
	u8		BoneIndices[4];
	E_VERTEX_TYPE getType() const { return EVT_BONEINDICES; }
};

inline u32 getVertexPitchFromType(E_VERTEX_TYPE vertexType)
{
	switch (vertexType)
	{
	case EVT_BASICCOLOR:
		return sizeof(S3DVertexBasicColor);
	case EVT_BASICTEX:
		return sizeof(S3DVertexBasicTex);
	case EVT_STANDARD:
		return sizeof(S3DVertex);
	case EVT_MODEL:
		return sizeof(S3DVertexModel);
	case EVT_BONEINDICES:
		return sizeof(S3DVertexBone);
	default:
		_ASSERT(false);
		return 0;
	}
}




