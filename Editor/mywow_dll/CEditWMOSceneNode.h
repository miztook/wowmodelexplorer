#pragma once

#include "CWMOSceneNode.h"
#include "editor_structs.h"

class CEditWMOSceneNode : public CWMOSceneNode
{
public:
	CEditWMOSceneNode(IFileWMO* wmo, ISceneNode* parent);
	virtual ~CEditWMOSceneNode();

public:
	virtual void render() const override;

public:
	void setOverrideWireframe(editor::E_OVERRIDE_WIREFRAME wireframe);
	void showGroup(u32 index, bool show);
	bool isGroupShow(u32 index);

	void showGroupBox(u32 index, bool show, SColor color);
	bool isGroupBoudingBoxShow(u32 index);
	void drawBoundingBox();

	void showPortal(u32 index, bool show, SColor color);
	bool isPortalShow(u32 index);
	void drawPortals();

	void showPortalGroups(u32 index, bool show);
	bool isPortalGroupsShow(u32 index);

public:
	bool	ShowWMO;

	struct SShowColor
	{
		bool show;
		SColor color;
	};

private:
	void renderWMOGroup(u32 groupIndex, u32 batchIndex) const;

	void setMaterial(const SWMOMaterial* material, SMaterial& mat) const;

private:
	editor::E_OVERRIDE_WIREFRAME		OverrideWireframe;
	bool*	ShowGroups;
	SShowColor*	ShowGroupsBoudingBox;
	SShowColor*	ShowPortals;
	bool*		ShowPortalGroups;
};