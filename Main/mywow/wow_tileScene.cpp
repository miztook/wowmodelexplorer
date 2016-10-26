#include "stdafx.h"
#include "wow_tileScene.h"
#include "mywow.h"
#include "CMapTileSceneNode.h"
#include "CFileADT.h"
#include "CFileWDT.h"
#include "CFileM2.h"

wow_tileScene::wow_tileScene( CMapTileSceneNode* mapTileNode ) : TileSceneNode(mapTileNode)
{
	FileWDT = TileSceneNode->FileWDT;

	CamChunk = LastCamChunk = NULL_PTR;

	u32 numM2Instance = TileSceneNode->Block.tile->fileAdt->NumM2Instance;
	u32 numWmoInstance = TileSceneNode->Block.tile->fileAdt->NumWmoInstance;

	//m2 instance
	M2InstSceneNodes = new SDynM2Inst[numM2Instance];
	memset(M2InstSceneNodes, 0, sizeof(SDynM2Inst) * numM2Instance);

	M2InstVisible = new bool[numM2Instance];
	memset(M2InstVisible, 0, sizeof(bool) * numM2Instance);

	//wmo instance
	WmoInstSceneNodes = new SDynWmoInst[numWmoInstance];
	memset(WmoInstSceneNodes, 0, sizeof(SDynWmoInst) * numWmoInstance);

	WmoInstVisible = new bool[numWmoInstance];
	memset(WmoInstVisible, 0, sizeof(bool) * numWmoInstance);
}

wow_tileScene::~wow_tileScene()
{
	delete[] WmoInstVisible;
	delete[] WmoInstSceneNodes;

	delete[] M2InstVisible;
	delete[] M2InstSceneNodes;
}

void wow_tileScene::addM2SceneNodes()
{
	g_Engine->getResourceLoader()->waitLoadingSuspend();

	CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);

	for (u32 i=0; i<adt->NumM2Instance; ++i)
	{
		const IFileADT::SM2Instance* instInfo = adt->getM2Instance(i);
		u32 index = instInfo->m2Index;

		const c8* filename = adt->getM2FileName(index);
		IFileM2* m2 = g_Engine->getResourceLoader()->loadM2(filename);
		if (!m2)
			continue;

		IM2SceneNode* m2SceneNode =g_Engine->getSceneManager()->addM2SceneNode(m2, TileSceneNode);
		const IFileADT::SM2Instance* inst = adt->getM2Instance(i);
		matrix4 mat;
// 		{
// 			vector3df pivot = m2->getBoundingAACenter();
// 			matrix4 global, inverseGlobal;
// 			global.setTranslation(pivot);
// 			inverseGlobal.setTranslation(-pivot);
// 			mat.setRotationDegrees(inst->dir);
// 
// 			mat = inverseGlobal * mat * global;
// 		}
		quaternion q(vector3df(PI/2, 0, 0));
		mat.setRotationDegrees(inst->dir);
		mat = q * mat;

		mat.setScale(inst->scale);
		mat.setTranslation(inst->position);
		m2SceneNode->setRelativeTransformation(mat);	
		m2SceneNode->update(true);
		m2SceneNode->buildVisibleGeosets();
		m2SceneNode->RenderInstType = ERT_DOODAD;
		m2SceneNode->enableFog(true);

		m2SceneNode->playAnimationByName("Stand", 0, true);

		M2InstSceneNodes[i].node = m2SceneNode;

		classifyM2Instance(i, mat);
	}	

}

void wow_tileScene::addWMOSceneNodes()
{
	g_Engine->getResourceLoader()->waitLoadingSuspend();

	CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);

	for (u32 i=0; i<adt->NumWmoInstance; ++i)
	{
		const IFileADT::SWmoInstance* instInfo = adt->getWMOInstance(i);
		u32 index = instInfo->wmoIndex;

		const c8* filename = adt->getWMOFileName(index);
		IFileWMO* wmo = g_Engine->getResourceLoader()->loadWMO(filename);
		if (!wmo)
			continue;

		IWMOSceneNode* wmoSceneNode =g_Engine->getSceneManager()->addWMOSceneNode(wmo, TileSceneNode);
		const IFileADT::SWmoInstance* inst = adt->getWMOInstance(i);
		matrix4 mat;
		mat.setRotationDegrees(inst->dir);
		mat.setScale(inst->scale);
		mat.setTranslation(inst->pos);
		wmoSceneNode->setRelativeTransformation(mat);
		wmoSceneNode->enableFog(true);

		WmoInstSceneNodes[i].node = wmoSceneNode;
	}	

}

void wow_tileScene::startLoadingM2SceneNodes()
{
	CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);

	IResourceLoader* resourceLoader = g_Engine->getResourceLoader();
	for (u32 i=0; i<adt->NumM2Instance; ++i)
	{
		const IFileADT::SM2Instance* instInfo = adt->getM2Instance(i);
		u32 index = instInfo->m2Index;

		const c8* filename = adt->getM2FileName(index);
		IResourceLoader::SParamBlock block = {0};
		block.param1 = TileSceneNode;
		block.param2 = (void*)(ptr_t)i;
		resourceLoader->beginLoadM2(filename, block);
	}
}

void wow_tileScene::startLoadingWMOSceneNodes()
{
	CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);

	IResourceLoader* resourceLoader = g_Engine->getResourceLoader();
	for (u32 i=0; i<adt->NumWmoInstance; ++i)
	{
		const IFileADT::SWmoInstance* instInfo = adt->getWMOInstance(i);
		u32 index = instInfo->wmoIndex;

		const c8* filename = adt->getWMOFileName(index);
		IResourceLoader::SParamBlock block = {0};
		block.param1 = TileSceneNode;
		block.param2 = (void*)(ptr_t)i;
		resourceLoader->beginLoadWMO(filename, block);
	}
}

void wow_tileScene::update( )
{
	processResources();

	//显示当前位置
	if (CamChunk != LastCamChunk)
	{
		if (CamChunk)
		{
			dbc::record r = g_Engine->getWowDatabase()->getAreaTableDB()->getByID(CamChunk->areaID);
			if (r.isValid())
				Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "%s %s", FileWDT->getMapName(), r.getString(areaTableDB::Name));
			else
				Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "%s", FileWDT->getMapName());
		}
		else
		{
			Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "%s", FileWDT->getMapName());
		}
		LastCamChunk = CamChunk;
	}

// 	vector3df v = g_Engine->getSceneManager()->getActiveCamera()->Position;
// 	f32 height;
// 	if (TileSceneNode->getHeight(v.X, v.Z, height))
// 	{
// 		Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "height: % 0.2f", height);
// 	}
// 	else
// 	{
// 		Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "height: 0");
// 	}
}

void wow_tileScene::registerInstances( ICamera* cam )
{
	registerVisibleM2Instances(cam);

	registerVisibleWmoInstances(cam);
}

void wow_tileScene::registerVisibleM2Instances( ICamera* cam )
{
	CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);
	frustum f = cam->getViewFrustum();

	f32 objectVisibleDistance = g_Engine->getSceneRenderServices()->getObjectVisibleDistance();
	f32 objectAlphaDistance = g_Engine->getSceneRenderServices()->getObjectAlphaDistance();

	//判断可见
	memset(M2InstVisible, 0, sizeof(bool) * adt->NumM2Instance);

	plane3df clipPlane = cam->getTerrainClipPlane();

	for (u8 i=0; i<16; ++i)
	{
		for (u8 j=0; j<16; ++j)
		{
			CMapTileSceneNode::SDynChunk* dynChunk = &TileSceneNode->Block.DynChunks[i][j];

			aabbox3df boxChunk = dynChunk->terrianWorldBox;
			boxChunk.MaxEdge.Y = FLT_MAX;
			if (!f.isInFrustum(boxChunk))
				continue;

			//小物体
			T_InstanceList& littleM2Instances = ChunkM2List[i][j].m2InstList;
			for (T_InstanceList::const_iterator itr=littleM2Instances.begin(); itr != littleM2Instances.end(); ++itr)
			{
				u32 idx = *itr;
				IM2SceneNode* node = M2InstSceneNodes[idx].node;
				if (!node || M2InstVisible[idx])		//已经可见，不在判断
					continue;

				aabbox3df box = node->getWorldBoundingAABox();

				vector3df center = box.getCenter();
				f32 radius = M2InstSceneNodes[idx].radius;
				if (radius == 0.0f)
					continue;

				f32 distance = cam->getPosition().getDistanceFrom(center) / radius;
				if(distance > objectVisibleDistance)			//过远
					continue;

				//不需要clipplane判断
				if (ISREL3D_BACK == box.classifyPlaneRelation(clipPlane) ||
					ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_LEFT_PLANE)) ||
					ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_RIGHT_PLANE)) ||
					ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_TOP_PLANE)) ||
					ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_BOTTOM_PLANE)) )
					continue;

				f32 delta = distance - objectAlphaDistance;
				if (delta > 0)
					node->setModelAlpha(true, 1.0f - delta / (objectVisibleDistance - objectAlphaDistance));
				else
					node->setModelAlpha(false, 1.0f);

				node->Visible = true;
				node->registerSceneNode(false, false);

				M2InstVisible[idx] = true;
			}
		}
	}
}

void wow_tileScene::registerVisibleWmoInstances( ICamera* cam )
{
	CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);
	frustum f = cam->getViewFrustum();

	//判断可见
	memset(WmoInstVisible, 0, sizeof(bool) * adt->NumWmoInstance);

	plane3df clipPlane = cam->getTerrainClipPlane();

	for (u32 i=0; i<adt->NumWmoInstance; ++i)
	{
		IWMOSceneNode* node = WmoInstSceneNodes[i].node;
		if (!node)
			continue;

		aabbox3df box = node->getWorldBoundingBox();

		if (ISREL3D_BACK == box.classifyPlaneRelation(clipPlane) ||
			ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_LEFT_PLANE)) ||
			ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_RIGHT_PLANE)) ||
			ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_TOP_PLANE)) ||
			ISREL3D_BACK == box.classifyPlaneRelation(f.getPlane(VF_BOTTOM_PLANE)) )
			continue;

		node->Visible = true;
		node->registerSceneNode(false, false);

		WmoInstVisible[i] = true;
	}
}

void wow_tileScene::classifyM2Instance( u32 index, const matrix4& instMat )
{
	CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);
	SDynM2Inst* m2Inst = &M2InstSceneNodes[index];
	const CFileM2* m2 = static_cast<const CFileM2*>(m2Inst->node->getFileM2());

	aabbox3df b = m2->getBoundingBox();
	instMat.transformBox(b);
	m2Inst->box = b;
	vector3df ext = b.getExtent();
	m2Inst->radius = (ext.X + ext.Y + ext.Z) / 3;

	rectf rcM2(b.MinEdge.X, b.MinEdge.Z, b.MaxEdge.X, b.MaxEdge.Z);

	for (u32 i=0; i<16; ++i)
	{
		for (u32 j=0; j<16; ++j)
		{
			aabbox3df box = adt->getChunk(i,j)->box;
			rectf rc(box.MinEdge.X, box.MinEdge.Z, box.MaxEdge.X, box.MaxEdge.Z);
			if (rcM2.intersectsWithRect(rc))
			{
				ChunkM2List[i][j].m2InstList.push_back(index);
			}
		}
	}
}

void wow_tileScene::processResources()
{
	IResourceLoader* loader = g_Engine->getResourceLoader();
	if(loader->m2LoadCompleted())
	{
		IResourceLoader::STask task = loader->getCurrentTask();
		ASSERT(task.type == IResourceLoader::ET_M2);
		IFileM2* filem2 = (IFileM2*)task.file;
		if(!filem2 || TileSceneNode != (IMapTileSceneNode*)task.param.param1)		//不是需要的m2
			return;
		u32 index = PTR_TO_UINT32(task.param.param2);
		
		loader->clearCurrentTask();
		loader->resumeLoading();

		CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);
		IM2SceneNode* m2SceneNode =g_Engine->getSceneManager()->addM2SceneNode(filem2, TileSceneNode);
		const IFileADT::SM2Instance* inst = adt->getM2Instance(index);
		matrix4 mat;
		mat.setRotationDegrees(inst->dir);
		mat.setScale(inst->scale);
		mat.setTranslation(inst->position);
		m2SceneNode->setRelativeTransformation(mat);
		m2SceneNode->update(true);
		m2SceneNode->buildVisibleGeosets();
		m2SceneNode->RenderInstType = ERT_DOODAD;
		m2SceneNode->enableFog(true);
		m2SceneNode->playAnimationByName("Stand", 0, true);

		M2InstSceneNodes[index].node = m2SceneNode;

		classifyM2Instance(index, mat);
	}

	if (loader->wmoLoadCompleted())
	{
		IResourceLoader::STask task = loader->getCurrentTask();
		ASSERT(task.type == IResourceLoader::ET_WMO);
		IFileWMO* filewmo = (IFileWMO*)task.file;
		if(!filewmo || TileSceneNode != (IMapTileSceneNode*)task.param.param1)		//不是需要的m2
			return;
		u32 index = PTR_TO_UINT32(task.param.param2);

		loader->clearCurrentTask();
		loader->resumeLoading();

		CFileADT* adt = static_cast<CFileADT*>(TileSceneNode->Block.tile->fileAdt);
		IWMOSceneNode* wmoSceneNode =g_Engine->getSceneManager()->addWMOSceneNode(filewmo, TileSceneNode);
		const IFileADT::SWmoInstance* inst =adt->getWMOInstance(index);
		matrix4 mat;
		mat.setRotationDegrees(inst->dir);
		mat.setTranslation(inst->pos);
		wmoSceneNode->setRelativeTransformation(mat);

		WmoInstSceneNodes[index].node = wmoSceneNode;
	}
}

