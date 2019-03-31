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

	virtual  void getCubeMeshVICount(uint32_t& vcount, uint32_t& icount) = 0;
	virtual  bool fillCubeMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, 
		const vector3df& size, SColor color = SColor()) = 0; 
	virtual  aabbox3df getCubeMeshAABBox(const vector3df& size) = 0;

	virtual  void getSphereMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t polyCountX, uint32_t polyCountY) = 0;
	virtual bool fillSphereMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		float radius = 5.f,
		uint32_t polyCountX = 16, uint32_t polyCountY = 16, SColor color = SColor()) = 0;
	virtual aabbox3df getSphereMeshAABBox(float radius) = 0;

	virtual void getSkyDomeMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t horiRes, uint32_t vertRes) = 0;
	virtual bool fillSkyDomeMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		uint32_t horiRes, uint32_t vertRes, float texturePercentage, float spherePercentage, float radius, SColor color = SColor()) = 0;
	virtual bool fillSkyDomeMeshVI(SVertex_PC* gVertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius, SColor color=SColor()) = 0;
	virtual aabbox3df getSkyDomeMeshAABBox(uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius) = 0;

	virtual uint32_t getGridLineVCount(uint32_t xzCount) = 0;
	virtual bool fillGridLineMeshV(SVertex_PC* gVertices, uint32_t vcount, uint32_t xzCount, float gridSize, SColor color) = 0;
	virtual aabbox3df getGridLineAABBox(uint32_t xzCount, float gridSize) = 0;
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