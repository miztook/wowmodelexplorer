#include "stdafx.h"
#include "wow_wmoScene.h"
#include "mywow.h"
#include "CWMOSceneNode.h"
#include "CFileWMO.h"

wow_wmoScene::wow_wmoScene( CWMOSceneNode* wmoNode )
	: WmoSceneNode(wmoNode), CameraIndoorGroupIndex(-1)
{
	FileWmo = static_cast<const CFileWMO*>(WmoSceneNode->getFileWMO());

	XOnePixel = YOnePixel = 0.0f;

	PortalChecked = new bool[FileWmo->Header.nPortals];
}

wow_wmoScene::~wow_wmoScene()
{
	delete[] PortalChecked;
}

s32 wow_wmoScene::getIndoorGroupIndexOfPosition( ICamera* cam, const vector3df& pos )
{
	u32 nGroups = FileWmo->Header.nGroups;
	for (u32 i=0; i<nGroups; ++i)
	{
		const CWMOGroup* group = &FileWmo->Groups[i];
		if(!group->indoor || group->NumBatches == 0)
			continue;

		const CWMOSceneNode::SDynGroup* dynGroup = &WmoSceneNode->DynGroups[i];
		if (dynGroup->visible && dynGroup->worldbox.isPointInside(pos))
		{
			//如果在portal以外，则认为是室外
			
			u32 frontCount = FileWmo->getPortalCountAsFront(i);
			for (u32 k=0; k<frontCount; ++k)		
			{
				s32 portalIndex = FileWmo->getPortalIndexAsFront(i, k);
				if (portalIndex != -1 && FileWmo->Portals[portalIndex].isValid())
				{
					const CWMOSceneNode::SDynPortal* dynPortal = &WmoSceneNode->DynPortals[portalIndex];
					EIntersectionRelation3D relation = dynPortal->worldplane.classifyPointRelation(pos);
					if (relation == ISREL3D_FRONT)
						return -1;
				}
			}

			u32 backCount = FileWmo->getPortalCountAsBack(i);
			for (u32 k=0; k<backCount; ++k)		
			{
				s32 portalIndex = FileWmo->getPortalIndexAsBack(i, k);
				if (portalIndex != -1 && FileWmo->Portals[portalIndex].isValid())
				{
					const CWMOSceneNode::SDynPortal* dynPortal = &WmoSceneNode->DynPortals[portalIndex];
					EIntersectionRelation3D relation = dynPortal->worldplane.classifyPointRelation(pos);
					if (relation == ISREL3D_BACK)
						return -1;
				}
			}

			//室内
			return (s32)i;
		}
	}
	return -1;
}

void wow_wmoScene::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	u32 nPortals = FileWmo->Header.nPortals;
	
	const recti& vp = g_Engine->getDriver()->getViewPort();
	XOnePixel = 2.0f / vp.getWidth();
	YOnePixel = 2.0f / vp.getHeight();

	VisibleGroups.clear();
	memset(PortalChecked, 0, sizeof(bool) * nPortals);

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	CameraIndoorGroupIndex = getIndoorGroupIndexOfPosition(cam, cam->getPosition());

	frustum f = cam->getViewFrustum();
	f.setFarPlane(cam->getTerrainClipPlane());

	//若是室外
	if (CameraIndoorGroupIndex == -1)
	{
		u32 nGroups = FileWmo->Header.nGroups;
		for (u32 i=0; i<nGroups; ++i)
		{
			const CWMOGroup* group = &FileWmo->Groups[i];
			const CWMOSceneNode::SDynGroup* dynGroup = &WmoSceneNode->DynGroups[i];
			if(!group->indoor && dynGroup->visible)
				VisibleGroups.insert(SGroupVisEntry(i, f));			//add all visible outdoor groups		
		}

		rectf rect(-1.0f, 1.0f, 1.0f, -1.0f);

		for (u32 i=0; i<nPortals; ++i)
		{
			const SWMOPortal* portal = &FileWmo->Portals[i];

			if (!portal->isValid())
				continue;
		
			const CWMOGroup* front = &FileWmo->Groups[portal->frontGroupIndex];
			const CWMOGroup* back = &FileWmo->Groups[portal->backGroupIndex];

			if (!front->indoor)
			{
				goThroughPortalFront(i, cam, f, rect, true);
			}
		
			if (!back->indoor)
			{
				goThroughPortalBack(i, cam, f, rect, true);
			}	
		}
	}
	else		//室内
	{
		rectf rect(-1.0f, 1.0f, 1.0f, -1.0f);

		VisibleGroups.insert(SGroupVisEntry(CameraIndoorGroupIndex, f));

		u32 frontCount = FileWmo->getPortalCountAsFront((u32)CameraIndoorGroupIndex);
		for (u32 i=0; i<frontCount; ++i)
		{
			s32 portalIndex = FileWmo->getPortalIndexAsFront((u32)CameraIndoorGroupIndex, i);
			if (portalIndex != -1 && FileWmo->Portals[portalIndex].isValid())
			{
				goThroughPortalFront(portalIndex, cam, f, rect, false);
			}
		}

		u32 backCount = FileWmo->getPortalCountAsBack((u32)CameraIndoorGroupIndex);
		for (u32 i=0; i<backCount; ++i)
		{
			s32 portalIndex = FileWmo->getPortalIndexAsBack((u32)CameraIndoorGroupIndex, i);
			if (portalIndex != -1 && FileWmo->Portals[portalIndex].isValid())
			{
				goThroughPortalBack(portalIndex, cam, f, rect, false);
			}
		}
	}

	//swprintf_s( g_Engine->getSceneManager()->DebugText, 512, L"%s, %d 个group判断", CameraIndoorGroupIndex == -1 ? L"室外" : L"室内",						VisibleGroups.size());
}


void wow_wmoScene::loadDoodadSceneNodes()
{
	unloadDoodadSceneNodes();

	for (u32 i=0; i<FileWmo->NumDoodads; ++i)
	{
		SWMODoodad* pWmoDoodad = &FileWmo->Doodads[i];

		string256 modelpath = pWmoDoodad->name;
		modelpath.changeExt("mdx", "m2");

		IM2SceneNode* node = NULL_PTR;
		IFileM2* pFileM2 = g_Engine->getResourceLoader()->loadM2(modelpath.c_str());
		if(pFileM2)
		{
			node = g_Engine->getSceneManager()->addM2SceneNode(pFileM2, WmoSceneNode);
			
			node->buildVisibleGeosets();

			matrix4 matRot;
			pWmoDoodad->quat.getMatrix(matRot);
			
			matrix4 mat;
			mat.setTranslation(pWmoDoodad->position);
			mat = pWmoDoodad->quat * mat;
			mat.setScale(pWmoDoodad->scale);

			node->setRelativeTransformation(mat);
		}
		
		DoodadSceneNodes.push_back(node);
	}

}

void wow_wmoScene::unloadDoodadSceneNodes()
{
	for(u32 i=0; i<(u32)DoodadSceneNodes.size(); ++i)
	{
		IM2SceneNode* node = DoodadSceneNodes[i];
		WmoSceneNode->removeChild(node);
	}
	DoodadSceneNodes.clear();
}

void wow_wmoScene::goThroughPortalFront( u32 index, ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor )
{
	const SWMOPortal* portal = &FileWmo->Portals[index];
	const CWMOSceneNode::SDynPortal* dynPortal = &WmoSceneNode->DynPortals[index];

	if (PortalChecked[index] ||
		!dynPortal->visible ||
		(onlyIndoor && !FileWmo->Groups[portal->backGroupIndex].indoor) ||
		cam->getDir().dotProduct(dynPortal->worldplane.Normal) < 0 || 
		!f.isInFrustum(dynPortal->worldbox))
		return;

	PortalChecked[index] = true;
	
	vector3df vmin = dynPortal->worldbox.MinEdge;
	vector3df vmax = dynPortal->worldbox.MaxEdge;

	f32 wmin, wmax;
	cam->getViewProjectionMatrix().transformVect(vmin, wmin);
	cam->getViewProjectionMatrix().transformVect(vmax, wmax);
	ASSERT(wmin != 0.0f && wmax != 0.0f);
	
	wmin = 1.0f / wmin;
	wmax = 1.0f / wmax;

	vmin *= wmin;
	vmax *= wmax;

	if (fabs(vmin.X - vmax.X) < XOnePixel || fabs(vmin.Y - vmax.Y) < YOnePixel)		//差距小于一个像素
		return;

	rectf rcPortal = rect;
	frustum clipFrustum = f;

	if (wmin > 0.0f && wmax > 0.0f)		//portal全部在near plane以外
	{
		f32 minz = min_(vmin.Z, vmax.Z);		//

		//portal剪切
		if(!clipPortal2D(rcPortal, vector2df(vmin.X, vmin.Y), vector2df(vmax.X, vmax.Y)))
			return;

		//计算portal的frustum
		makeFrustum(clipFrustum, cam, rcPortal.UpperLeftCorner.X, rcPortal.UpperLeftCorner.Y, rcPortal.LowerRightCorner.X, rcPortal.LowerRightCorner.Y, minz);
	}

	VisibleGroups.insert(SGroupVisEntry(portal->backGroupIndex, clipFrustum));			
	
	//find back -> font, 继续找相连的portal
	u32 frontCount = FileWmo->getPortalCountAsFront(portal->backGroupIndex);
	for (u32 i=0; i<frontCount; ++i)
	{
		s32 portalIndex = FileWmo->getPortalIndexAsFront(portal->backGroupIndex, i);
		if (portalIndex != -1)
			goThroughPortalFront((u32)portalIndex, cam, clipFrustum, rcPortal, onlyIndoor);
	}
}

void wow_wmoScene::goThroughPortalBack( u32 index, ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor )
{
	const SWMOPortal* portal = &FileWmo->Portals[index];
	const CWMOSceneNode::SDynPortal* dynPortal = &WmoSceneNode->DynPortals[index];

	if (PortalChecked[index] ||
		!dynPortal->visible ||
		(onlyIndoor && !FileWmo->Groups[portal->frontGroupIndex].indoor) ||
		cam->getDir().dotProduct(dynPortal->worldplane.Normal) > 0 || 
		!f.isInFrustum(dynPortal->worldbox))
		return;

	PortalChecked[index] = true;

	vector3df vmin = dynPortal->worldbox.MinEdge;
	vector3df vmax = dynPortal->worldbox.MaxEdge;

	f32 wmin, wmax;
	cam->getViewProjectionMatrix().transformVect(vmin, wmin);
	cam->getViewProjectionMatrix().transformVect(vmax, wmax);
	ASSERT(wmin != 0.0f && wmax != 0.0f);

	wmin = 1.0f / wmin;
	wmax = 1.0f / wmax;

	vmin *= wmin;
	vmax *= wmax;

	if (fabs(vmin.X - vmax.X) < XOnePixel || fabs(vmin.Y - vmax.Y) < YOnePixel)		//差距小于一个像素
		return;

	rectf rcPortal = rect;
	frustum clipFrustum = f;

	if (wmin > 0.0f && wmax > 0.0f)		//portal全部在near plane以外
	{
		f32 minz = min_(vmin.Z, vmax.Z);		//

		//portal剪切
		if(!clipPortal2D(rcPortal, vector2df(vmin.X, vmin.Y), vector2df(vmax.X, vmax.Y)))
			return;

		//计算portal的frustum
		makeFrustum(clipFrustum, cam, rcPortal.UpperLeftCorner.X, rcPortal.UpperLeftCorner.Y, rcPortal.LowerRightCorner.X, rcPortal.LowerRightCorner.Y, minz);
	}

	VisibleGroups.insert(SGroupVisEntry(portal->frontGroupIndex, clipFrustum));			

	//find front -> back, 继续找相连的portal
	u32 backCount = FileWmo->getPortalCountAsBack(portal->frontGroupIndex);
	for (u32 i=0; i<backCount; ++i)
	{
		s32 portalIndex = FileWmo->getPortalIndexAsBack(portal->frontGroupIndex, i);
		if (portalIndex != -1)
			goThroughPortalBack((u32)portalIndex, cam, clipFrustum, rcPortal, onlyIndoor);
	}
}

bool wow_wmoScene::clipPortal2D( rectf& rect, const vector2df& vmin, const vector2df& vmax )
{
	f32 maxX, maxY, minX, minY;
	if (vmin.X > vmax.X)
	{ 
		maxX = vmin.X; minX = vmax.X; 
	}
	else
	{ 
		maxX = vmax.X; minX = vmin.X; 
	}

	if (vmin.Y > vmax.Y)
	{
		maxY = vmin.Y; minY = vmax.Y;
	}
	else
	{
		maxY = vmax.Y; minY = vmin.Y;
	}

	//clip rect
	if (maxX < rect.UpperLeftCorner.X || minX > rect.LowerRightCorner.X ||
		maxY < rect.LowerRightCorner.Y || minY > rect.UpperLeftCorner.Y)
	return false;

	if (rect.UpperLeftCorner.X < minX) rect.UpperLeftCorner.X = minX;
	if (rect.UpperLeftCorner.Y > maxY) rect.UpperLeftCorner.Y = maxY;
	if (rect.LowerRightCorner.X > maxX) rect.LowerRightCorner.X = maxX;
	if (rect.LowerRightCorner.Y < minY) rect.LowerRightCorner.Y = minY;

	return true;
}

void wow_wmoScene::makeFrustum( frustum& f, ICamera* cam, f32 left, f32 top, f32 right, f32 bottom, f32 z )
{
	vector3df vleftTop(left, top, z);
	vector3df vLeftBottom(left, bottom, z);
	vector3df vRightTop(right, top, z);
	vector3df vRightBottom(right, bottom, z);

	matrix4 mat = cam->getInverseViewProjectionMatrix();

	mat.transformVect(vleftTop);
	mat.transformVect(vLeftBottom);
	mat.transformVect(vRightTop);
	mat.transformVect(vRightBottom);

	f.setFrom(cam->getPosition(), vleftTop, vLeftBottom, vRightTop, vRightBottom);
}
