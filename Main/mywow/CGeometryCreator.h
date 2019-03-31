#pragma once

#include "IGeometryCreator.h"

class CGeometryCreator : public IGeometryCreator
{
public:
	CGeometryCreator() {}

public:
	virtual  void getCubeMeshVICount(uint32_t& vcount, uint32_t& icount);
	virtual  bool fillCubeMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, 
		const vector3df& size, SColor color = SColor()); 
	virtual  aabbox3df getCubeMeshAABBox(const vector3df& size) { return aabbox3df(-size/2, size/2); }

	virtual  void getSphereMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t polyCountX, uint32_t polyCountY);
	virtual bool fillSphereMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		float radius = 5.f,
		uint32_t polyCountX = 16, uint32_t polyCountY = 16, SColor color = SColor());
	virtual aabbox3df getSphereMeshAABBox(float radius) { return aabbox3df(-radius, -radius, -radius, radius, radius, radius); }

	virtual void getSkyDomeMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t horiRes, uint32_t vertRes);
	virtual bool fillSkyDomeMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		uint32_t horiRes, uint32_t vertRes, float texturePercentage, float spherePercentage, float radius, SColor color = SColor());
	virtual bool fillSkyDomeMeshVI(SVertex_PC* gVertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius, SColor color=SColor());
	virtual aabbox3df getSkyDomeMeshAABBox(uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius);

	virtual uint32_t getGridLineVCount(uint32_t xzCount)  { return (2*xzCount+1) * 4; }
	virtual bool fillGridLineMeshV(SVertex_PC* vertices, uint32_t vcount, uint32_t xzCount, float gridSize, SColor color);
	virtual aabbox3df getGridLineAABBox(uint32_t xzCount, float gridSize)
	{
		float halfWidth = xzCount * gridSize;
		return aabbox3df(-halfWidth,0, -halfWidth, halfWidth,0,halfWidth); 
	}

};