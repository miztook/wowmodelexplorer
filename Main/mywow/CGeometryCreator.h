#pragma once

#include "IGeometryCreator.h"

class CGeometryCreator : public IGeometryCreator
{
public:
	CGeometryCreator() {}

public:
	virtual  void getCubeMeshVICount(u32& vcount, u32& icount);
	virtual  bool fillCubeMeshVI(SVertex_PCT* vertices, u32 vcount, u16* indices, u32 icount, 
		const vector3df& size, SColor color = SColor()); 
	virtual  aabbox3df getCubeMeshAABBox(const vector3df& size) { return aabbox3df(-size/2, size/2); }

	virtual  void getSphereMeshVICount(u32& vcount, u32& icount, u32 polyCountX, u32 polyCountY);
	virtual bool fillSphereMeshVI(SVertex_PCT* vertices, u32 vcount, u16* indices, u32 icount,
		f32 radius = 5.f,
		u32 polyCountX = 16, u32 polyCountY = 16, SColor color = SColor());
	virtual aabbox3df getSphereMeshAABBox(f32 radius) { return aabbox3df(-radius, -radius, -radius, radius, radius, radius); }

	virtual void getSkyDomeMeshVICount(u32& vcount, u32& icount, u32 horiRes, u32 vertRes);
	virtual bool fillSkyDomeMeshVI(SVertex_PCT* vertices, u32 vcount, u16* indices, u32 icount,
		u32 horiRes, u32 vertRes, f32 texturePercentage, f32 spherePercentage, f32 radius, SColor color = SColor());
	virtual bool fillSkyDomeMeshVI(SVertex_PC* gVertices, u32 vcount, u16* indices, u32 icount,
		u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius, SColor color=SColor());
	virtual aabbox3df getSkyDomeMeshAABBox(u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius);

	virtual u32 getGridLineVCount(u32 xzCount)  { return (2*xzCount+1) * 4; }
	virtual bool fillGridLineMeshV(SVertex_PC* vertices, u32 vcount, u32 xzCount, f32 gridSize, SColor color);
	virtual aabbox3df getGridLineAABBox(u32 xzCount, f32 gridSize)
	{
		f32 halfWidth = xzCount * gridSize;
		return aabbox3df(-halfWidth,0, -halfWidth, halfWidth,0,halfWidth); 
	}

};