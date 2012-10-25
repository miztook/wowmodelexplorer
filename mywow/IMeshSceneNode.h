#pragma once

#include "ISceneNode.h"
#include "SMaterial.h"

class IMesh;
class ITexture;

class IMeshSceneNode : public ISceneNode
{
public:
	IMeshSceneNode(ISceneNode* parent)
		: ISceneNode(parent), Billboard(false), RenderInstType(ERT_MESH) {}

	virtual ~IMeshSceneNode() {}

public:
	virtual IMesh* getMesh() const  = 0;
	virtual void setTexture(u32 i, ITexture* texture) = 0;
	virtual SMaterial& getMaterial() = 0;

	void setRenderInstType(E_RENDERINST_TYPE renderType) { RenderInstType = renderType; }

	E_RENDERINST_TYPE		RenderInstType;
	bool		Billboard;
};