#pragma once

#include "core.h"
#include "SColor.h"
#include "S3DVertex.h"

class IGeometryCreator
{
public:
	virtual ~IGeometryCreator() {}

public:
	virtual E_PRIMITIVE_TYPE getPrimType(E_GEOMETRY_TYPE type);

	virtual  void getCubeMeshVICount(u32& vcount, u32& icount) = 0;
	virtual  bool fillCubeMeshVI(S3DVertexBasicTex* vertices, u32 vcount, u16* indices, u32 icount, 
		const vector3df& size, SColor color = SColor()) = 0; 
	virtual  aabbox3df getCubeMeshAABBox(const vector3df& size) = 0;

	virtual  void getSphereMeshVICount(u32& vcount, u32& icount, u32 polyCountX, u32 polyCountY) = 0;
	virtual bool fillSphereMeshVI(S3DVertexBasicTex* vertices, u32 vcount, u16* indices, u32 icount,
		f32 radius = 5.f,
		u32 polyCountX = 16, u32 polyCountY = 16, SColor color = SColor()) = 0;
	virtual aabbox3df getSphereMeshAABBox(f32 radius) = 0;

	virtual void getSkyDomeMeshVICount(u32& vcount, u32& icount, u32 horiRes, u32 vertRes) = 0;
	virtual bool fillSkyDomeMeshVI(S3DVertexBasicTex* vertices, u32 vcount, u16* indices, u32 icount,
		u32 horiRes, u32 vertRes, f32 texturePercentage, f32 spherePercentage, f32 radius, SColor color = SColor()) = 0;

	virtual u32 getGridLineVCount(u32 xzCount) = 0;
	virtual bool fillGridLineMeshV(S3DVertexBasicColor* vertices, u32 vcount, u32 xzCount, f32 gridSize, SColor color) = 0;
	virtual aabbox3df getGridLineAABBox(u32 xzCount, f32 gridSize) = 0;
};

inline E_PRIMITIVE_TYPE IGeometryCreator::getPrimType( E_GEOMETRY_TYPE type )
{
	switch (type)
	{
	case EGT_CUBE:
	case EGT_SPHERE:
	case EGT_SKYDOME:
		return EPT_TRIANGLES;
	case EGT_GRID:
		return EPT_LINES;
	default:
		return EPT_TRIANGLES;
	}
}