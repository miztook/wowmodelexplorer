#pragma once

#include "core.h"
#include "SColor.h"

class IMesh;
class IVertexBuffer;
class IIndexBuffer;

class IManualMeshServices
{
public:
	virtual ~IManualMeshServices() {}

public:
	virtual bool addMesh(string64 name, IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box) = 0;
	virtual IMesh* getMesh(string64 name) = 0;
	virtual void removeMesh(string64 name) = 0;
	virtual void clear() = 0;

	//
	virtual bool addGridLineMesh(string64 name, u32 xzCount, f32 gridSize, SColor color) = 0;
	virtual bool addDecal(string64 name, f32 width, f32 height, SColor color) = 0;
	virtual bool addSphere(string64 name, f32 radius, u32 polyCountX, u32 polyCountY, SColor color) = 0;
};
