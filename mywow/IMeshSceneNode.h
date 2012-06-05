#pragma once

#include "ISceneNode.h"
#include "SMaterial.h"

class IMesh;
class ITexture;

class IMeshSceneNode : public ISceneNode
{
public:
	IMeshSceneNode(ISceneNode* parent, E_RENDERINST_TYPE renderType = ERT_MESH)
		: ISceneNode(parent, renderType), Billboard(false) {}

	virtual ~IMeshSceneNode() {}

public:
	virtual IMesh* getMesh() const  = 0;
	virtual void setTexture(u32 i, ITexture* texture) = 0;
	virtual SMaterial& getMaterial() = 0;

	bool		Billboard;
};