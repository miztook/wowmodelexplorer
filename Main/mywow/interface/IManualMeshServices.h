#pragma once

#include "core.h"
#include "SColor.h"

class IMesh;
struct SBufferParam;

class IManualMeshServices
{
public:
	virtual ~IManualMeshServices() {}

public:
	virtual bool addMesh(const char* name, const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box) = 0;
	virtual IMesh* getMesh(const char* name) = 0;
	virtual void removeMesh(const char* name) = 0;
	virtual void clear() = 0;

	//
	virtual bool addGridLineMesh(const char* name, uint32_t xzCount, float gridSize, SColor color) = 0;
	virtual bool addDecal(const char* name, float width, float height, SColor color) = 0;
	virtual bool addCube(const char* name, const vector3df& size, SColor color) = 0;
	virtual bool addSphere(const char* name, float radius, uint32_t polyCountX, uint32_t polyCountY, SColor color) = 0;
	virtual bool addSkyDome(const char* name, uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius, SColor color) = 0;
};
