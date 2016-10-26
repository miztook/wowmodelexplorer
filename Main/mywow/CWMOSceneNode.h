#pragma once

#include "IWMOSceneNode.h"
#include "SColor.h"

class CWMOGroup;
struct SWMOMaterial;
struct SMaterial;
class wow_wmoScene;
class CFileWMO;

class CWMOSceneNode : public IWMOSceneNode
{
public:
	CWMOSceneNode(IFileWMO* wmo, ISceneNode* parent);
	virtual ~CWMOSceneNode();

public:
	//IWMOSceneNode
	virtual const IFileWMO* getFileWMO() const { return (IFileWMO*)Wmo; }
	virtual void enableFog(bool enable);

	virtual void loadDoodadSceneNodes();
	virtual void unloadDoodadSceneNodes();

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const;
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const;
	virtual void onUpdated();

protected:
	struct	SDynBatch
	{
		aabbox3df worldbox;
		bool visible;
	};

	struct SDynPortal
	{
		aabbox3df worldbox;
		plane3df	worldplane;
		rectf		rect2d;
		bool		visible;
	};

	struct SDynGroup
	{
		~SDynGroup() { delete[] batches; }

		aabbox3df		worldbox;
		f32	 distancesq;
		SDynBatch*		batches;
		bool visible;
	};

protected:
	void renderWMOGroup(u32 groupIndex, u32 batchIndex) const;

	void setMaterial(const SWMOMaterial* material, SMaterial& mat) const;

	//for editor
	virtual void drawPortal(u32 index, SColor color);

	virtual void drawBspNode(u32 groupIndex, u32 nodeIndex, SColor color);

protected:
	CFileWMO*  Wmo;
	SDynGroup*		DynGroups;
	SDynPortal*		DynPortals;
	wow_wmoScene*	WmoScene;
	bool EnableFog;

	//friend
	friend class wow_wmoScene;
};