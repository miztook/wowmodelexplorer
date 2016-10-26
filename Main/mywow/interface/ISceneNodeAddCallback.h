#pragma once

class IM2SceneNode;

class IM2SceneNodeAddCallback
{
public:
	virtual void onM2SceneNodeAdd(IM2SceneNode* node) = 0;
};