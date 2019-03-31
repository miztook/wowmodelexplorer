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
	virtual bool addMesh(const char* name, const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box);
	virtual IMesh* getMesh(const char* name);
	virtual void removeMesh(const char* name);
	virtual void clear();

	virtual bool addGridLineMesh(const char* name, uint32_t xzCount, float gridSize, SColor color);
	virtual bool addDecal(const char* name, float width, float height, SColor color);
	virtual bool addCube(const char* name, const vector3df& size, SColor color);
	virtual bool addSphere(const char* name, float radius, uint32_t polyCountX, uint32_t polyCountY, SColor color);
	virtual bool addSkyDome(const char* name, uint32_t horiRes, uint32_t vertRes, float spherePercentage, float radius, SColor color);

private:
#ifdef USE_QALLOCATOR
	typedef std::map<string64, IMesh*, std::less<string64>, qzone_allocator<std::pair<string64, IMesh*>>> TMeshMap;
#else
	typedef std::unordered_map<string64, IMesh*, string64::string_hash> TMeshMap;
#endif

	TMeshMap		MeshMap;
};