#include "stdafx.h"
#include "editor_WMOSceneNode.h"
#include "CEditWMOSceneNode.h"
#include "CFileWMO.h"

void WMOSceneNode_showEditPart( IWMOSceneNode* node, editor::E_WMO_EDIT_PART editpart, bool show )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	switch (editpart)
	{
	case EWP_WMO:
		editNode->ShowWMO = show;
		break;
	}
}

void WMOSceneNode_setWireframe( IWMOSceneNode* node, editor::E_OVERRIDE_WIREFRAME wireframe )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	editNode->setOverrideWireframe(wireframe);
}

void WMOSceneNode_getFileWMO( IWMOSceneNode* node, SFileWMO* filewmo )
{
	const IFileWMO* wmo = node->getFileWMO();

	getFileNameA(wmo->Name, filewmo->name, DEFAULT_SIZE);
	strcpy_s(filewmo->longname, 256, wmo->getFileName());
	filewmo->numMaterials = wmo->Header.nMaterials;
	filewmo->numGroups = wmo->Header.nGroups;
	filewmo->numPortals = wmo->Header.nPortals;
	filewmo->numLights = wmo->Header.nLights;
	filewmo->numModels = wmo->Header.nModels;
	filewmo->numDoodads = wmo->NumDoodads;
	filewmo->numDoodadSets = wmo->Header.nDoodadSets;
	filewmo->wmoID = wmo->Header.wmoID;
}

bool WMOSceneNode_getGroup( IWMOSceneNode* node, u32 index, editor::SWMOGroup* group )
{
	const IFileWMO* fileWmo = node->getFileWMO();
	if (index >= fileWmo->Header.nGroups)
		return false;

	Q_strcpy(group->name, DEFAULT_SIZE, fileWmo->Groups[index].name);
	group->numBatches = fileWmo->Groups[index].NumBatches;
	group->numDoodads = fileWmo->Groups[index].NumDoodads;
	group->numLights = fileWmo->Groups[index].NumLights;
	group->VCount = fileWmo->Groups[index].VCount;
	group->ICount = fileWmo->Groups[index].ICount;
	group->Indoor = fileWmo->Groups[index].indoor;

	return true;
}

void WMOSceneNode_showGroup( IWMOSceneNode* node, u32 index, bool show )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	editNode->showGroup(index, show);
}

bool WMOSceneNode_isGroupShow( IWMOSceneNode* node, u32 index )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	return editNode->isGroupShow(index);
}

void WMOSceneNode_showGroupBox( IWMOSceneNode* node, u32 index, bool show, SColor color )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	editNode->showGroupBox(index, show, color);
}

bool WMOSceneNode_isGroupBoxShow( IWMOSceneNode* node, u32 index )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	return editNode->isGroupBoudingBoxShow(index);
}

bool WMOSceneNode_getPortal( IWMOSceneNode* node, u32 index, editor::SWMOPortal* portal )
{
	const IFileWMO* wmo = node->getFileWMO();
	if (index >= wmo->Header.nPortals)
		return false;

	portal->frontGroupIndex = wmo->Portals[index].frontGroupIndex;
	portal->backGroupIndex = wmo->Portals[index].backGroupIndex;

	return true;
}

void WMOSceneNode_showPortal( IWMOSceneNode* node, u32 index, bool show, SColor color )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	editNode->showPortal(index, show, color);
}

bool WMOSceneNode_isPortalShow( IWMOSceneNode* node, u32 index )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	return editNode->isPortalShow(index);
}

void WMOSceneNode_showPortalGroups( IWMOSceneNode* node, u32 index, bool show )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	editNode->showPortalGroups(index, show);
}

bool WMOSceneNode_isPortalGroupsShow( IWMOSceneNode* node, u32 index )
{
	CEditWMOSceneNode* editNode = (CEditWMOSceneNode*)node;
	return editNode->isPortalGroupsShow(index);
}

bool WMOSceneNode_getDoodadName(IWMOSceneNode* node, u32 index, c8* filename, u32 size)
{
	const IFileWMO* wmo = node->getFileWMO();
	if (index >= wmo->Header.nDoodads)
		return false;

	string512 modelpath = wmo->Doodads[index].name;
	modelpath.normalize();
	modelpath.make_lower();
	modelpath.changeExt("mdx", "m2");
	if (size < modelpath.length() + 1)
		return false;

	Q_strcpy(filename, size, modelpath.c_str());
	return true;
}
