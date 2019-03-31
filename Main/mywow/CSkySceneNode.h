#pragma once

#include "ISkySceneNode.h"
#include "SMaterial.h"

class CMapEnvironment;
class IWDTSceneNode;
class CMesh;
struct SVertex_PC;

class CSkySceneNode : public ISkySceneNode
{
public:
	explicit CSkySceneNode(CMapEnvironment* mapEnvironment);
	virtual ~CSkySceneNode();

public:
	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const { return aabbox3df::Zero(); }
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const { return true; }

protected:
	void createSkyDomeMesh();
	bool fillSkyDomeMeshVI(SVertex_PC* gVertices, uint32_t vcount, uint16_t* indices, uint32_t icount);
	void updateMeshColor();

protected:
	SMaterial		Material;
	CMapEnvironment*		MapEnvironment;
	//sky dome
	CMesh*		SkyDomeMesh;
	float			SkyRadius;	

	matrix4		WorldMatrix;
};