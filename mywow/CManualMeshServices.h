#pragma once

#include "IManualMeshServices.h"
#include <map>

class CManualMeshServices : public IManualMeshServices
{
public:
	~CManualMeshServices();

public:
	virtual bool addMesh(string64 name, const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box);
	virtual IMesh* getMesh(string64 name);
	virtual void removeMesh(string64 name);
	virtual void clear();

	virtual bool addGridLineMesh(string64 name, u32 xzCount, f32 gridSize, SColor color);
	virtual bool addDecal(string64 name, f32 width, f32 height, SColor color);
	virtual bool addSphere(string64 name, f32 radius, u32 polyCountX, u32 polyCountY, SColor color);
	virtual bool addSkyDome(string64 name, u32 horiRes, u32 vertRes, f32 spherePercentage, f32 radius, SColor color);

private:
	typedef std::map<string64, IMesh*, std::less<string64>, qzone_allocator<std::pair<string64, IMesh*>>> TMeshMap;
	TMeshMap		MeshMap;
};