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

int32_t wow_wmoScene::getIndoorGroupIndexOfPosition( const ICamera* cam, const vector3df& pos )
{
	uint32_t nGroups = FileWmo->Header.nGroups;
	for (uint32_t i=0; i<nGroups; ++i)
	{
		const CWMOGroup* group = &FileWmo->Groups[i];
		if(!group->indoor || group->NumBatches == 0)
			continue;

		const CWMOSceneNode::SDynGroup* dynGroup = &WmoSceneNode->DynGroups[i];
		if (dynGroup->visible && dynGroup->worldbox.isPointInside(pos))
		{
			//如果在portal以外，则认为是室外
			
			uint32_t frontCount = FileWmo->getPortalCountAsFront(i);
			for (uint32_t k=0; k<frontCount; ++k)		
			{
				int32_t portalIndex = FileWmo->getPortalIndexAsFront(i, k);
				if (portalIndex != -1 && FileWmo->Portals[portalIndex].isValid())
				{
					const CWMOSceneNode::SDynPortal* dynPortal = &WmoSceneNode->DynPortals[portalIndex];
					EIntersectionRelation3D relation = dynPortal->worldplane.classifyPointRelation(pos);
					if (relation == ISREL3D_FRONT)
						return -1;
				}
			}

			uint32_t backCount = FileWmo->getPortalCountAsBack(i);
			for (uint32_t k=0; k<backCount; ++k)		
			{
				int32_t portalIndex = FileWmo->getPortalIndexAsBack(i, k);
				if (portalIndex != -1 && FileWmo->Portals[portalIndex].isValid())
				{
					const CWMOSceneNode::SDynPortal* dynPortal = &WmoSceneNode->DynPortals[portalIndex];
					EIntersectionRelation3D relation = dynPortal->worldplane.classifyPointRelation(pos);
					if (relation == ISREL3D_BACK)
						return -1;
				}
			}

			//室内
			return (int32_t)i;
		}
	}
	return -1;
}

void wow_wmoScene::tick( uint32_t timeSinceStart, uint32_t timeSinceLastFrame )
{
	uint32_t nPortals = FileWmo->Header.nPortals;
	
	const recti& vp = g_Engine->getDriver()->getViewPort();
	XOnePixel = 2.0f / vp.getWidth();
	YOnePixel = 2.0f / vp.getHeight();

	VisibleGroups.clear();
	memset(PortalChecked, 0, sizeof(bool) * nPortals);

	const ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	CameraIndoorGroupIndex = getIndoorGroupIndexOfPosition(cam, cam->getPosition());

	frustum f = cam->getViewFrustum();
	f.setFarPlane(cam->getTerrainClipPlane());

	//若是室外
	if (CameraIndoorGroupIndex == -1)
	{
		uint32_t nGroups = FileWmo->Header.nGroups;
		for (uint32_t i=0; i<nGroups; ++i)
		{
			const CWMOGroup* group = &FileWmo->Groups[i];
			const CWMOSceneNode::SDynGroup* dynGroup = &WmoSceneNode->DynGroups[i];
			if(!group->indoor && dynGroup->visible)
				VisibleGroups.insert(SGroupVisEntry(i, f));			//add all visible outdoor groups		
		}

		rectf rect(-1.0f, 1.0f, 1.0f, -1.0f);

		for (uint32_t i=0; i<nPortals; ++i)
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

		uint32_t frontCount = FileWmo->getPortalCountAsFront((uint32_t)CameraIndoorGroupIndex);
		for (uint32_t i=0; i<frontCount; ++i)
		{
			int32_t portalIndex = FileWmo->getPortalIndexAsFront((uint32_t)CameraIndoorGroupIndex, i);
			if (portalIndex != -1 && FileWmo->Portals[portalIndex].isValid())
			{
				goThroughPortalFront(portalIndex, cam, f, rect, false);
			}
		}

		uint32_t backCount = FileWmo->getPortalCountAsBack((uint32_t)CameraIndoorGroupIndex);
		for (uint32_t i=0; i<backCount; ++i)
		{
			int32_t portalIndex = FileWmo->getPortalIndexAsBack((uint32_t)CameraIndoorGroupIndex, i);
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

	for (uint32_t i=0; i<FileWmo->NumDoodads; ++i)
	{
		SWMODoodad* pWmoDoodad = &FileWmo->Doodads[i];

		string256 modelpath = pWmoDoodad->name;
		modelpath.changeExt("mdx", "m2");

		IM2SceneNode* node = nullptr;
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
	for(uint32_t i=0; i<(uint32_t)DoodadSceneNodes.size(); ++i)
	{
		IM2SceneNode* node = DoodadSceneNodes[i];
		WmoSceneNode->removeChild(node);
	}
	DoodadSceneNodes.clear();
}

void wow_wmoScene::goThroughPortalFront( uint32_t index, const ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor )
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

	float wmin, wmax;
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
		float minz = min_(vmin.Z, vmax.Z);		//

		//portal剪切
		if(!clipPortal2D(rcPortal, vector2df(vmin.X, vmin.Y), vector2df(vmax.X, vmax.Y)))
			return;

		//计算portal的frustum
		makeFrustum(clipFrustum, cam, rcPortal.UpperLeftCorner.X, rcPortal.UpperLeftCorner.Y, rcPortal.LowerRightCorner.X, rcPortal.LowerRightCorner.Y);
	}

	VisibleGroups.insert(SGroupVisEntry(portal->backGroupIndex, clipFrustum));			
	
	//find back -> font, 继续找相连的portal
	uint32_t frontCount = FileWmo->getPortalCountAsFront(portal->backGroupIndex);
	for (uint32_t i=0; i<frontCount; ++i)
	{
		int32_t portalIndex = FileWmo->getPortalIndexAsFront(portal->backGroupIndex, i);
		if (portalIndex != -1)
			goThroughPortalFront((uint32_t)portalIndex, cam, clipFrustum, rcPortal, onlyIndoor);
	}
}

void wow_wmoScene::goThroughPortalBack( uint32_t index, const ICamera* cam, const frustum& f, const rectf& rect, bool onlyIndoor )
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

	float wmin, wmax;
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
		float minz = min_(vmin.Z, vmax.Z);		//

		//portal剪切
		if(!clipPortal2D(rcPortal, vector2df(vmin.X, vmin.Y), vector2df(vmax.X, vmax.Y)))
			return;

		//计算portal的frustum
		makeFrustum(clipFrustum, cam, rcPortal.UpperLeftCorner.X, rcPortal.UpperLeftCorner.Y, rcPortal.LowerRightCorner.X, rcPortal.LowerRightCorner.Y);
	}

	VisibleGroups.insert(SGroupVisEntry(portal->frontGroupIndex, clipFrustum));			

	//find front -> back, 继续找相连的portal
	uint32_t backCount = FileWmo->getPortalCountAsBack(portal->frontGroupIndex);
	for (uint32_t i=0; i<backCount; ++i)
	{
		int32_t portalIndex = FileWmo->getPortalIndexAsBack(portal->frontGroupIndex, i);
		if (portalIndex != -1)
			goThroughPortalBack((uint32_t)portalIndex, cam, clipFrustum, rcPortal, onlyIndoor);
	}
}

bool wow_wmoScene::clipPortal2D( rectf& rect, const vector2df& vmin, const vector2df& vmax )
{
	float maxX, maxY, minX, minY;
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

void wow_wmoScene::makeFrustum(frustum& f, const ICamera* cam, float left, float top, float right, float bottom)
{
	vector3df vleftTop(left, top, bottom);
	vector3df vLeftBottom(left, bottom, bottom);
	vector3df vRightTop(right, top, bottom);
	vector3df vRightBottom(right, bottom, bottom);

	matrix4 mat = cam->getInverseViewProjectionMatrix();

	mat.transformVect(vleftTop);
	mat.transformVect(vLeftBottom);
	mat.transformVect(vRightTop);
	mat.transformVect(vRightBottom);

	f.setFrom(cam->getPosition(), vleftTop, vLeftBottom, vRightTop, vRightBottom);
}
