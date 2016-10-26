#pragma once

#include "IManualMeshServices.h"
#include <unordered_map>

class CManualMeshServices : public IManualMeshServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CManualMeshServices);

public:
	CManualMeshServices() {}
	~CManualMeshServices();

public:
	virtual bool addMesh(const c8* name, const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box);
	virtual IMesh* getMesh(const c8* name);
	virtual void removeMesh(const c8* name);
	virtual void clear();

	virtual bool addGridLineMesh(const c8* name, u32 xzCount, f32 gridSize, SColor color);
	virtual bool addDecal(const c8* name, f32 width, f32 height, SColor color);
	virtual bool addCube(const c8* name, const vector3df& size, SColor color);
	virtual bool addSphere(const c8* name, f32 radius, u32 polyCountX, u32 polyCountY, SColor color);
	virtual bool addSkyDome(const c8* name, u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius, SColor color);

private:
	typedef std::unordered_map<string64, IMesh*, string64::string_hash> TMeshMap;
	TMeshMap		MeshMap;
};