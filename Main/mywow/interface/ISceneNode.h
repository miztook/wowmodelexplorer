#pragma once

#include "core.h"
#include <list>

enum E_SCENENODE_TYPE
{
	EST_NONE = 0,
	EST_MESH,
	EST_M2,
	EST_WMO,
	EST_MAPTILE,
	EST_WDT,
	EST_SKY,
	EST_PARTICLE,
	EST_RIBBON,
	EST_COORD,
};

class ISceneNode
{
public:
	explicit ISceneNode(ISceneNode* parent)
		: Parent(parent), Distance(0), Type(EST_NONE), Generation(0)
	{
		InitializeListHead(&Link);
		InitializeListHead(&ChildNodeList);

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
	const matrix4& getAbsoluteRotateMatrix() const { return AbsoluteRotateMatrix; }
	const matrix4& getRelativeRotateMatrix() const { return RelativeRotateMatrix; }
	const matrix4& getAbsoluteTransformation() const { return AbsoluteTransformation; }
	const matrix4& getRelativeTransformation() const { return RelativeTransformation; }
	void setRelativeTransformation(const matrix4& mat) 
	{
		vector3df s = mat.getScale();
		RelativeRotateMatrix = mat;
		RelativeRotateMatrix.setScale(vector3df(1.0f/s.X, 1.0f/s.Y, 1.0f/s.Z));
		RelativeRotateMatrix.setTranslation(vector3df::Zero());

		RelativeTransformation = mat; 
		NeedUpdate = true; 
	}

	//
	vector3df getDir() const { return vector3df(RelativeRotateMatrix.M[8], RelativeRotateMatrix.M[9], RelativeRotateMatrix.M[10]); }
	vector3df getUp() const { return vector3df(RelativeRotateMatrix.M[4], RelativeRotateMatrix.M[5], RelativeRotateMatrix.M[6]); }
	void setDir(const vector3df& dir) { setDirAndUp(dir, getUp()); }
	void setUp(const vector3df& up) { setDirAndUp(getDir(), up); }
	void setDirAndUp(const vector3df& dir, const vector3df& up);
	vector3df getPos() const { return RelativeTransformation.getTranslation(); }
	void setPos(const vector3df& pos) { RelativeTransformation.setTranslation(pos); NeedUpdate = true; } 
	vector3df getScale() const { return RelativeTransformation.getScale(); }
	void setScale(const vector3df& scale);
	void setScale(float scale) { return setScale(vector3df(scale, scale, scale)); }

	vector3df getAbsDir() const { return vector3df(AbsoluteRotateMatrix.M[8], AbsoluteRotateMatrix.M[9], AbsoluteRotateMatrix.M[10]); }
	vector3df getAbsUp() const { return vector3df(AbsoluteRotateMatrix.M[4], AbsoluteRotateMatrix.M[5], AbsoluteRotateMatrix.M[6]); }
	vector3df getAbsPos() const { return AbsoluteTransformation.getTranslation(); }
	vector3df getAbsScale() const { return AbsoluteTransformation.getScale(); }

	void rotateAxisY(float radians) { quaternion q(radians, vector3df::UnitY()); rotate(q);}
	void move(const vector3df& offset) { setPos(getPos() + offset); }

	void update(bool includeChildren = true);
	void updateAABB();
	void addChild(ISceneNode* child);
	bool removeChild(ISceneNode* child);
	bool hasChild() const;
	void removeAllChildren();
	E_SCENENODE_TYPE getType() const { return (E_SCENENODE_TYPE)Type; }

	virtual void registerSceneNode(bool frustumcheck,  int sequence);
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible) {}
	virtual void render() const = 0;
	virtual aabbox3df getBoundingBox() const  = 0;
	const aabbox3df& getWorldBoundingBox() const { return WorldBoundingBox; }

	virtual bool isNodeEligible() const = 0;

protected:
	virtual void onUpdated() { updateAABB(); }
	void rotate(const quaternion& q);

public:
	LENTRY		Link;
	LENTRY		ChildNodeList;			//child lists
	ISceneNode*			Parent;
	f32  Distance;		//ºÍÉãÏñ»úµÄ¾àÀë

protected:
	matrix4			RelativeRotateMatrix;
	matrix4			RelativeTransformation;
	matrix4			AbsoluteRotateMatrix;
	matrix4			AbsoluteTransformation;
	aabbox3df		WorldBoundingBox;
public:
	bool		NeedUpdate;
	bool		Visible;
	u8	Type;
	u8	Generation;
};

inline void ISceneNode::update( bool includeChildren /*= true*/ )
{
	if ( NeedUpdate )
	{
		if ( Parent )
		{
			AbsoluteRotateMatrix = getRelativeRotateMatrix() * Parent->AbsoluteRotateMatrix;
			AbsoluteTransformation = getRelativeTransformation() * Parent->AbsoluteTransformation;
		}
		else
		{
			AbsoluteRotateMatrix = getRelativeRotateMatrix();
			AbsoluteTransformation = getRelativeTransformation();
		}

	//	ASSERT(!AbsoluteTransformation.equals(matrix4::Zero()));

		onUpdated();

		if ( includeChildren )
		{
			for (PLENTRY e = ChildNodeList.Flink; e != &ChildNodeList;  e = e->Flink)
			{
				ISceneNode* node = reinterpret_cast<ISceneNode*>CONTAINING_RECORD(e, ISceneNode, Link);
				node->NeedUpdate = true;
				node->update(includeChildren);
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
	ASSERT(child && child!=this);

	if (child->Parent && child->Parent != this)
		child->Parent->removeChild(this);

	InsertTailList(&ChildNodeList, &child->Link);
	child->Parent = this;

	child->Generation = Generation + 1;
}

inline bool ISceneNode::removeChild( ISceneNode* child )
{
	ASSERT(child && child!=this);

	for (PLENTRY e = ChildNodeList.Flink; e != &ChildNodeList; )
	{
		ISceneNode* node = reinterpret_cast<ISceneNode*>CONTAINING_RECORD(e, ISceneNode, Link);
		e = e->Flink;
		if(node == child)
		{
			RemoveEntryList(&child->Link);
			child->Parent = NULL_PTR;

			return true;
		}
	}

	return false;
}

inline void ISceneNode::removeAllChildren()
{
	for (PLENTRY e = ChildNodeList.Flink; e != &ChildNodeList; )
	{
		ISceneNode* node = reinterpret_cast<ISceneNode*>CONTAINING_RECORD(e, ISceneNode, Link);
		e = e->Flink;

		node->removeAllChildren();
		node->Parent = NULL_PTR;
		delete node;
	}
	InitializeListHead(&ChildNodeList);
}

inline bool ISceneNode::hasChild() const
{
	return !IsListEmpty(&ChildNodeList);
}

inline void ISceneNode::registerSceneNode(bool frustumcheck, int sequence)
{
	for (PLENTRY e = ChildNodeList.Flink; e != &ChildNodeList;  e = e->Flink)
	{
		ISceneNode* node = reinterpret_cast<ISceneNode*>CONTAINING_RECORD(e, ISceneNode, Link);

		node->registerSceneNode(frustumcheck, sequence);
	}
}

inline void ISceneNode::setDirAndUp( const vector3df& dir, const vector3df& up )
{
	RelativeRotateMatrix.buildMatrix(dir, up, vector3df::Zero());
	matrix4 mat = RelativeRotateMatrix;
	mat.setScale(getScale());
	mat.setTranslation(getPos());

	RelativeTransformation = mat; 
	NeedUpdate = true; 
}

inline void ISceneNode::setScale( const vector3df& scale )
{
	matrix4 mat = RelativeRotateMatrix;
	mat.setScale(scale);
	mat.setTranslation(getPos());

	RelativeTransformation = mat; 
	NeedUpdate = true;
}

inline void ISceneNode::rotate( const quaternion& q )
{
	RelativeRotateMatrix = q * RelativeRotateMatrix;
	matrix4 mat = RelativeRotateMatrix;
	mat.setScale(getScale());
	mat.setTranslation(getPos());

	RelativeTransformation = mat; 
	NeedUpdate = true; 
}
