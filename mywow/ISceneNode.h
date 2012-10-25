#pragma once

#include "core.h"
#include <list>

class ISceneNode
{
public:
	ISceneNode(ISceneNode* parent)
		: Parent(parent), Generation(0), DistanceSq(0)
	{
		if (!Parent)
			Generation = 0;
		else
			Generation = Parent->Generation + 1;

		if (Parent)
			Parent->addChild(this);

		NeedUpdate = true;
		Visible = true;
	}

	virtual ~ISceneNode() {}
public:
	const matrix4& getAbsoluteTransformation() const { return AbsoluteTransformation; }
	const matrix4& getRelativeTransformation() const { return RelativeTransformation; }
	void setRelativeTransformation(const matrix4& mat) {	RelativeTransformation = mat; NeedUpdate = true; }
	void update(bool includeChildren = true);
	void updateAABB();
	void addChild(ISceneNode* child);
	void removeChild(ISceneNode* child);
	void removeAllChildren();

	virtual void registerSceneNode(bool frustumcheck = true);
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame) {}
	virtual void onPreRender() {}
	virtual void render() = 0;
	virtual aabbox3df getBoundingBox() const  = 0;
	const aabbox3df& getWorldBoundingBox() { return WorldBoundingBox; }

	virtual bool isNodeEligible() const { return false; }

protected:
	virtual void onUpdated() { updateAABB(); }

public:
	typedef std::list<ISceneNode*, qzone_allocator<ISceneNode*>>		SceneNodeList;

	ISceneNode*			Parent;
	SceneNodeList			ChildNodes;
	u8	Generation;

	f32  DistanceSq;		//ºÍÉãÏñ»úµÄ¾àÀësquare

protected:
	bool		NeedUpdate;
	matrix4			RelativeTransformation;
	matrix4			AbsoluteTransformation;
	aabbox3df		WorldBoundingBox;

public:
	bool			Visible;
};

inline void ISceneNode::update( bool includeChildren /*= true*/ )
{
	if ( NeedUpdate )
	{
		if ( Parent )
		{
			AbsoluteTransformation = Parent->AbsoluteTransformation * getRelativeTransformation();
		}
		else
			AbsoluteTransformation = getRelativeTransformation();

		onUpdated();

		if ( includeChildren )
		{
			for (SceneNodeList::iterator itr= ChildNodes.begin(); itr != ChildNodes.end(); ++itr)
			{
				(*itr)->NeedUpdate = true;
				(*itr)->update(includeChildren);
			}
		}
		NeedUpdate = false;
	}
}

inline void ISceneNode::updateAABB()
{
	WorldBoundingBox = getBoundingBox();
	if(!WorldBoundingBox.isZero())
		AbsoluteTransformation.transformBox(WorldBoundingBox);
}

inline void ISceneNode::addChild( ISceneNode* child )
{
	_ASSERT(child && child!=this);

	if (child->Parent && child->Parent != this)
		child->Parent->removeChild(child);

	ChildNodes.push_back(child);
	child->Parent = this;

	child->Generation = Generation + 1;
}

inline void ISceneNode::removeChild( ISceneNode* child )
{
	_ASSERT(child && child!=this);

	for (SceneNodeList::iterator itr= ChildNodes.begin(); itr != ChildNodes.end(); ++itr)
	{
		if ((*itr)==child)
		{
			child->Parent = NULL;
			ChildNodes.erase(itr);
			child->removeAllChildren();
			delete child;
			break;
		}
	}
}

inline void ISceneNode::removeAllChildren()
{
	for (SceneNodeList::iterator itr= ChildNodes.begin(); itr != ChildNodes.end(); ++itr)
	{
		ISceneNode* node = *itr;
		node->removeAllChildren();
		node->Parent = NULL;
		delete node;
	}
	ChildNodes.clear();
}

inline void ISceneNode::registerSceneNode(bool frustumcheck)
{
	for(SceneNodeList::iterator itr= ChildNodes.begin(); itr != ChildNodes.end(); ++itr)
	{
		(*itr)->registerSceneNode(frustumcheck);
	}
}
