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
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const { return true; }

protected:
	void createSkyDomeMesh();
	bool fillSkyDomeMeshVI(SVertex_PC* gVertices, u32 vcount, u16* indices, u32 icount);
	void updateMeshColor();

protected:
	SMaterial		Material;
	CMapEnvironment*		MapEnvironment;
	//sky dome
	CMesh*		SkyDomeMesh;
	f32			SkyRadius;	

	matrix4		WorldMatrix;
};