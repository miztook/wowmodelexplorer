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
	virtual bool addMesh(const c8* name, const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box) = 0;
	virtual IMesh* getMesh(const c8* name) = 0;
	virtual void removeMesh(const c8* name) = 0;
	virtual void clear() = 0;

	//
	virtual bool addGridLineMesh(const c8* name, u32 xzCount, f32 gridSize, SColor color) = 0;
	virtual bool addDecal(const c8* name, f32 width, f32 height, SColor color) = 0;
	virtual bool addCube(const c8* name, const vector3df& size, SColor color) = 0;
	virtual bool addSphere(const c8* name, f32 radius, u32 polyCountX, u32 polyCountY, SColor color) = 0;
	virtual bool addSkyDome(const c8* name, u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius, SColor color) = 0;
};
