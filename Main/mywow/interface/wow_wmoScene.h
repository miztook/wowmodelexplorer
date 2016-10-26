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
	void tick(u32 timeSinceStart, u32 timeSinceLastFrame);

	void loadDoodadSceneNodes();
	void unloadDoodadSceneNodes();

private:
	//portal
	s32 getIndoorGroupIndexOfPosition(ICamera* cam, const vector3df& pos);
	
	void goThroughPortalFront(u32 index, ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor);
	void goThroughPortalBack(u32 index, ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor);

	bool clipPortal2D(rectf& rect, const vector2df& vmin, const vector2df& vmax);
	void makeFrustum(frustum& f, ICamera* cam, f32 left, f32 top, f32 right, f32 z, f32 bottom);

	//

private:

	struct SGroupVisEntry		//每个group中的小物体判断可见的frustum
	{
		SGroupVisEntry(u32 index, const frustum& f) : groupIndex(index), frust(f) {}

		u32 groupIndex;
		frustum	frust;

		bool operator<(const SGroupVisEntry& other) const { return groupIndex < other.groupIndex; }
	};

private:
	CWMOSceneNode*	WmoSceneNode;
	const CFileWMO*		FileWmo;
	s32		CameraIndoorGroupIndex;

	typedef std::set<SGroupVisEntry, std::less<SGroupVisEntry>, qzone_allocator<SGroupVisEntry> > T_VisibleGroups;
	T_VisibleGroups	VisibleGroups;

	std::vector<IM2SceneNode*>		DoodadSceneNodes;

	bool*	PortalChecked;

	f32	XOnePixel;
	f32	YOnePixel;
};