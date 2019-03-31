#pragma once

#include "core.h"

class CWMOSceneNode;
class IM2SceneNode;
class ICamera;
class CFileWMO;

//wmo的场景管理
class wow_wmoScene
{
public:
	explicit wow_wmoScene(CWMOSceneNode* wmoNode);
	~wow_wmoScene();

public:
	void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame);

	void loadDoodadSceneNodes();
	void unloadDoodadSceneNodes();

private:
	//portal
	int32_t getIndoorGroupIndexOfPosition(ICamera* cam, const vector3df& pos);
	
	void goThroughPortalFront(uint32_t index, ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor);
	void goThroughPortalBack(uint32_t index, ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor);

	bool clipPortal2D(rectf& rect, const vector2df& vmin, const vector2df& vmax);
	void makeFrustum(frustum& f, ICamera* cam, float left, float top, float right, float z, float bottom);

	//

private:

	struct SGroupVisEntry		//每个group中的小物体判断可见的frustum
	{
		SGroupVisEntry(uint32_t index, const frustum& f) : groupIndex(index), frust(f) {}

		uint32_t groupIndex;
		frustum	frust;

		bool operator<(const SGroupVisEntry& other) const { return groupIndex < other.groupIndex; }
	};

private:
	CWMOSceneNode*	WmoSceneNode;
	const CFileWMO*		FileWmo;
	int32_t		CameraIndoorGroupIndex;

	typedef std::set<SGroupVisEntry, std::less<SGroupVisEntry>, qzone_allocator<SGroupVisEntry> > T_VisibleGroups;
	T_VisibleGroups	VisibleGroups;

	std::vector<IM2SceneNode*>		DoodadSceneNodes;

	bool*	PortalChecked;

	float	XOnePixel;
	float	YOnePixel;
};