#pragma once

#include "IMeshSceneNode.h"

class CMeshSceneNode : public IMeshSceneNode
{
public:
	CMeshSceneNode(IMesh* mesh, ISceneNode* parent);
	~CMeshSceneNode();

public:
	//IMeshSceneNode
	virtual IMesh* getMesh() const { return Mesh; }
	virtual void setTexture(u32 i, ITexture* texture);
	virtual SMaterial& getMaterial() { return Material; }

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const;
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const;

private:
	IMesh*		Mesh;
	
	SMaterial		Material;
	ITexture*		Textures[MATERIAL_MAX_TEXTURES];

	matrix4		Mat;
};