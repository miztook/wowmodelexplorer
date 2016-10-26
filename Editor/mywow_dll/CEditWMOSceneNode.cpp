#include "stdafx.h"
#include "CEditWMOSceneNode.h"
#include "CFileWMO.h"
#include "CSceneRenderServices.h"

CEditWMOSceneNode::CEditWMOSceneNode( IFileWMO* wmo, ISceneNode* parent )
	: CWMOSceneNode(wmo, parent), ShowGroups(NULL), ShowGroupsBoudingBox(NULL)
{
	ShowWMO = true;

	OverrideWireframe = editor::EOW_NONE;

	ShowGroups = new bool[Wmo->Header.nGroups];
	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
		ShowGroups[i] = true;

	ShowGroupsBoudingBox = new SShowColor[Wmo->Header.nGroups];
	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
		ShowGroupsBoudingBox[i].show = false;

	ShowPortals = new SShowColor[Wmo->Header.nPortals];
	ShowPortalGroups = new bool[Wmo->Header.nPortals];
	for (u32 i=0; i<Wmo->Header.nPortals; ++i)
	{
		ShowPortals[i].show = false;
		ShowPortalGroups[i] = false;
	}
}

CEditWMOSceneNode::~CEditWMOSceneNode()
{
	delete[] ShowPortalGroups;
	delete[] ShowPortals;
	delete[] ShowGroupsBoudingBox;
	delete[] ShowGroups;
}

void CEditWMOSceneNode::render() const
{
	//wmo
	if (ShowWMO)
	{
		for (u32 i=0; i<Wmo->Header.nGroups; ++i)
		{
			const CWMOGroup* group = &Wmo->Groups[i];
			if (!DynGroups[i].visible || !ShowGroups[i])
				continue;

			for (u32 c=0; c<group->NumBatches; ++c)
			{
				renderWMOGroup(i, c);
			}
		}
	}
	
	//doodads

	//liquid
}

void CEditWMOSceneNode::renderWMOGroup( u32 groupIndex, u32 batchIndex ) const
{
	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	CFileWMO* wmo = static_cast<CFileWMO*>(Wmo);
	CWMOGroup* group = &Wmo->Groups[groupIndex];
	SDynGroup* dynGroup = &DynGroups[groupIndex];
	const SWMOBatch* batch = &group->Batches[batchIndex];
	u16 matId = batch->matId;
	_ASSERT(matId < Wmo->Header.nMaterials);

	const SWMOMaterial* material = &Wmo->Materials[matId]; 

	SRenderUnit unit = {0};

	setMaterial(material, unit.material);

	unit.distance = dynGroup->distancesq;
	unit.bufferParam.vbuffer0 = wmo->VertexBuffer;
	unit.bufferParam.ibuffer = wmo->IndexBuffer;
	unit.bufferParam.vType = EVT_PNCT2;
	unit.primType = EPT_TRIANGLES;
	unit.primCount = batch->indexCount / 3;
	unit.drawParam.startIndex = group->IStart + batch->indexStart;
	unit.drawParam.minVertIndex = group->VStart + batch->vertexStart;
	unit.drawParam.numVertices = batch->getVertexCount();
	unit.sceneNode = this;
	unit.matWorld = &AbsoluteTransformation;
	unit.textures[0] = material->texture0;
	unit.textures[1] = material->texture1;

	sceneRenderServices->addRenderUnit(&unit, ERT_WMO);
}

void CEditWMOSceneNode::setMaterial( const SWMOMaterial* material, SMaterial& mat ) const
{
	CWMOSceneNode::setMaterial(material, mat);

	//wireframe
	switch(OverrideWireframe)
	{
	case editor::EOW_NONE:
		mat.Wireframe = false;
		break;
	case editor::EOW_WIREFRAME:
		mat.Wireframe = true;
		break;
	case editor::EOW_WIREFRAME_SOLID:
		mat.Wireframe = true;
		mat.MaterialType = EMT_SOLID;
		break;
	case editor::EOW_WIREFRAME_ONECOLOR:
		mat.Wireframe = true;
		mat.MaterialType = EMT_LINE;
		break;
	}
}

void CEditWMOSceneNode::setOverrideWireframe( editor::E_OVERRIDE_WIREFRAME wireframe )
{
	OverrideWireframe = wireframe;
}

void CEditWMOSceneNode::showGroup( u32 index, bool show )
{
	if (index >= Wmo->Header.nGroups)
		return;

	ShowGroups[index] = show;
}

bool CEditWMOSceneNode::isGroupShow( u32 index )
{
	if (index >= Wmo->Header.nGroups)
		return false;

	return ShowGroups[index];
}

void CEditWMOSceneNode::drawBoundingBox()
{
	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
	{
		if (!DynGroups[i].visible || !ShowGroupsBoudingBox[i].show)
			continue;
	
		g_Engine->getDrawServices()->addAABB(DynGroups[i].worldbox, ShowGroupsBoudingBox[i].color);
	}
}

void CEditWMOSceneNode::showGroupBox( u32 index, bool show, SColor color )
{
	if (index >= Wmo->Header.nGroups)
		return;

	ShowGroupsBoudingBox[index].show = show;
	ShowGroupsBoudingBox[index].color = color;
}

bool CEditWMOSceneNode::isGroupBoudingBoxShow( u32 index )
{
	if (index >= Wmo->Header.nGroups)
		return false;

	return ShowGroupsBoudingBox[index].show;
}

void CEditWMOSceneNode::showPortal( u32 index, bool show, SColor color )
{
	if (index >= Wmo->Header.nPortals)
		return;

	ShowPortals[index].show = show;
	ShowPortals[index].color = color;
}

bool CEditWMOSceneNode::isPortalShow( u32 index )
{
	if (index >= Wmo->Header.nPortals)
		return false;

	return ShowPortals[index].show;
}

void CEditWMOSceneNode::drawPortals()
{
	for (u32 i=0; i<Wmo->Header.nPortals; ++i)
	{
		const SDynPortal* portal = &DynPortals[i];
		if (!portal->visible || !ShowPortals[i].show)
			continue;
		
		drawPortal(i, ShowPortals[i].color);
	}
}

void CEditWMOSceneNode::showPortalGroups( u32 index, bool show )
{
	if (index >= Wmo->Header.nPortals)
		return;

	ShowPortalGroups[index] = show;

	const SWMOPortal* portal = &Wmo->Portals[index];
	showGroupBox(portal->frontGroupIndex, show, SColor(0,255,0));
	showGroupBox(portal->backGroupIndex, show, SColor(0,0, 255));
}

bool CEditWMOSceneNode::isPortalGroupsShow( u32 index )
{
	if (index >= Wmo->Header.nPortals)
		return false;

	return ShowPortalGroups[index];
}
