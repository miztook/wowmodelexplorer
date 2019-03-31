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
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible);
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
		float	 distancesq;
		SDynBatch*		batches;
		bool visible;
	};

protected:
	void renderWMOGroup(uint32_t groupIndex, uint32_t batchIndex) const;

	void setMaterial(const SWMOMaterial* material, SMaterial& mat) const;

	//for editor
	virtual void drawPortal(uint32_t index, SColor color);

	virtual void drawBspNode(uint32_t groupIndex, uint32_t nodeIndex, SColor color);

protected:
	CFileWMO*  Wmo;
	SDynGroup*		DynGroups;
	SDynPortal*		DynPortals;
	wow_wmoScene*	WmoScene;
	bool EnableFog;

	//friend
	friend class wow_wmoScene;
};