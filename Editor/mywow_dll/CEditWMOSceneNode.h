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
	void showGroup(uint32_t index, bool show);
	bool isGroupShow(uint32_t index);

	void showGroupBox(uint32_t index, bool show, SColor color);
	bool isGroupBoudingBoxShow(uint32_t index);
	void drawBoundingBox();

	void showPortal(uint32_t index, bool show, SColor color);
	bool isPortalShow(uint32_t index);
	void drawPortals();

	void showPortalGroups(uint32_t index, bool show);
	bool isPortalGroupsShow(uint32_t index);

public:
	bool	ShowWMO;

	struct SShowColor
	{
		bool show;
		SColor color;
	};

private:
	void renderWMOGroup(uint32_t groupIndex, uint32_t batchIndex) const;

	void setMaterial(const SWMOMaterial* material, SMaterial& mat) const;

private:
	editor::E_OVERRIDE_WIREFRAME		OverrideWireframe;
	bool*	ShowGroups;
	SShowColor*	ShowGroupsBoudingBox;
	SShowColor*	ShowPortals;
	bool*		ShowPortalGroups;
};