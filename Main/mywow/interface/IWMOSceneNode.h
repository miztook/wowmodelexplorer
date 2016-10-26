#pragma once

#include "ISceneNode.h"

class IFileWMO;

class IWMOSceneNode : public ISceneNode
{
public:
	explicit IWMOSceneNode(ISceneNode* parent)
		: ISceneNode(parent){ Type = EST_WMO; }

	virtual ~IWMOSceneNode() { }

public:
	virtual const IFileWMO* getFileWMO() const = 0;

	virtual void enableFog(bool enable) = 0;

	virtual void loadDoodadSceneNodes() = 0;
	virtual void unloadDoodadSceneNodes() = 0;
};