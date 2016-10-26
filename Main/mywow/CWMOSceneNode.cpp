#include "stdafx.h"
#include "CWMOSceneNode.h"
#include "mywow.h"
#include "CFileWMO.h"
#include "CMesh.h"
#include "wow_wmoScene.h"
#include "CSceneRenderServices.h"

CWMOSceneNode::CWMOSceneNode( IFileWMO* wmo, ISceneNode* parent )
	: IWMOSceneNode(parent)
{
	Wmo = static_cast<CFileWMO*>(wmo);

	ASSERT(Wmo);
	Wmo->grab();
	
	DynGroups = new SDynGroup[Wmo->Header.nGroups];
	memset(DynGroups, 0, sizeof(SDynGroup) * Wmo->Header.nGroups);

	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
	{
		DynGroups[i].batches = new SDynBatch[Wmo->Groups[i].NumBatches];
		memset(DynGroups[i].batches, 0, sizeof(SDynBatch) * Wmo->Groups[i].NumBatches);
	}

	DynPortals = new SDynPortal[Wmo->Header.nPortals];
	memset(DynPortals, 0, sizeof(SDynPortal) * Wmo->Header.nPortals);

// 	matrix4 mat;
// 	mat.setScale(g_Engine->getSceneRenderServices()->SCENE_SCALE);
// 	setRelativeTransformation(mat);
// 	update();

	EnableFog = true;

	//scene
	WmoScene = new wow_wmoScene(this);
}

CWMOSceneNode::~CWMOSceneNode()
{
	delete WmoScene;

	delete[] DynPortals;
	delete[] DynGroups;

	Wmo->drop();
}

void CWMOSceneNode::registerSceneNode(bool frustumcheck, int sequence)
{
	update();

	if (!Visible ||
		(frustumcheck && !isNodeEligible()))
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);

	IWMOSceneNode::registerSceneNode(frustumcheck, sequence);

	//
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	frustum f = cam->getViewFrustum();
	f.setFarPlane(cam->getTerrainClipPlane());

	//find visible group & batch
	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
	{
		aabbox3df box = DynGroups[i].worldbox;
			
		if (!f.isInFrustum(box))
		{
			DynGroups[i].visible = false;
			continue;
		}

		DynGroups[i].visible = true;
		DynGroups[i].distancesq = cam->getPosition().getDistanceFromSQ(DynGroups[i].worldbox.getCenter());

		for (u32 k=0; k<Wmo->Groups[i].NumBatches; ++k)
		{
			SDynBatch* batch = &DynGroups[i].batches[k];
			box = batch->worldbox;

			batch->visible = f.isInFrustum(box);
		}
	}

	//find visible portals
	for (u32 i=0; i<Wmo->Header.nPortals; ++i)
	{
		SDynPortal* dynPortal = &DynPortals[i];

		aabbox3df box = dynPortal->worldbox;
		dynPortal->visible = f.isInFrustum(box);
	}
}

aabbox3df CWMOSceneNode::getBoundingBox() const
{
	return Wmo->getBoundingBox();
}

void CWMOSceneNode::render() const
{
	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
	{
		const CWMOGroup* group = &Wmo->Groups[i];
		if (!DynGroups[i].visible)
			continue;
	
		for (u32 c=0; c<group->NumBatches; ++c)
		{
			if (DynGroups[i].batches[c].visible)
				renderWMOGroup(i, c);
		}
	}


	//doodads

	//liquid

	/*
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	for (u32 i=0; i<Wmo->Header.nPortals; ++i)
	{
		const SWMOPortal* portal = &Wmo->Portals[i];
		const SDynPortal* dynPortal = &DynPortals[i];
		if (!dynPortal->visible)
			continue;

		bool face = cam->getDir().dotProduct(dynPortal->worldplane.Normal) > 0;

		if(face)
			drawPortal(i, SColor(0,255,0));
		else
			drawPortal(i, SColor(255,0,0));
	}
	*/
	/*
	for (u32 i=1; i<2; ++i)
	{
		const CWMOGroup* group = &Wmo->Groups[i];
		if (!DynGroups[i].visible)
			continue;
		for (u32 c=0; c<group->NumBspNodes; ++c)
			drawBspNode(i, c, SColor(255,255,0));

		g_Engine->getDrawServices()->add3DBox(DynGroups[i].worldbox, SColor(255,0,0));
	}
	*/
}

void CWMOSceneNode::renderWMOGroup( u32 groupIndex, u32 batchIndex ) const
{
	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	CFileWMO* wmo = static_cast<CFileWMO*>(Wmo);
	CWMOGroup* group = &Wmo->Groups[groupIndex];
	SDynGroup* dynGroup = &DynGroups[groupIndex];
	const SWMOBatch* batch = &group->Batches[batchIndex];
	u16 matId = batch->matId;
	ASSERT(matId < Wmo->Header.nMaterials);

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

bool CWMOSceneNode::isNodeEligible() const
{
	aabbox3df box = getWorldBoundingBox();

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	if (!cam)
		return false;
	return cam->getViewFrustum().isInFrustum(box);
}


void CWMOSceneNode::onUpdated()
{
	IWMOSceneNode::onUpdated();

	//group
	for (u32 i=0; i<Wmo->Header.nGroups; ++i)
	{
		const CWMOGroup* group = &Wmo->Groups[i];
		SDynGroup* dynGroup = &DynGroups[i];
		dynGroup->worldbox =	group->box;
		AbsoluteTransformation.transformBox(dynGroup->worldbox);
		for (u32 k=0; k<group->NumBatches; ++k)
		{
			dynGroup->batches[k].worldbox = group->Batches[k].box;
			AbsoluteTransformation.transformBox(dynGroup->batches[k].worldbox);
		}
	}

	//portal
	for (u32 i=0; i<Wmo->Header.nPortals; ++i)
	{
		const SWMOPortal* portal = &Wmo->Portals[i];
		SDynPortal* dynPortal = &DynPortals[i];
		dynPortal->worldbox = portal->box;
		AbsoluteTransformation.transformBox(dynPortal->worldbox);
		dynPortal->worldplane = portal->plane;
		AbsoluteTransformation.transformPlane(dynPortal->worldplane);
	}
}

void CWMOSceneNode::setMaterial( const SWMOMaterial* material, SMaterial& mat ) const
{
	IShaderServices* shaderServices = g_Engine->getDriver()->getShaderServices();
	switch(material->shaderType)
	{
	case Diffuse:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1);
			mat.PsType = EPST_MAPOBJ_DIFFUSE;
		}
		break;
	case Specular:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_SPECULAR_T1);
			mat.PsType = EPST_MAPOBJ_SPECULAR;
		}
		break;
	case Metal:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_SPECULAR_T1);
			mat.PsType = EPST_MAPOBJ_METAL;
		}
		break;
	case Env:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1_REFL);
			mat.PsType = EPST_MAPOBJ_ENV;
		}
		break;
	case Opaque:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1);
			mat.PsType = EPST_MAPOBJ_OPAQUE;
		}
		break;
	case EnvMetal:
		{
 			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1_ENV_T2);
			mat.PsType = EPST_MAPOBJ_ENVMETAL;
		}
		break;
	case TwoLayerDiffuse:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1_T2);
			mat.PsType = EPST_MAPOBJ_TWOLAYERDIFFUSE;
		}
		break;
	case TwoLayerEnvMetal:			//
		{
  			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_SPECULAR_T1);
			mat.PsType = EPST_MAPOBJ_METAL;
		}
		break;
	case TwoLayerTerrain:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1_T2);
			mat.PsType = EPST_MAPOBJ_TWOLAYERTERRAIN;
		}
		break;
	case DiffuseEmissive:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1);
			mat.PsType = EPST_MAPOBJ_DIFFUSEEMISSIVE;
		}
		break;
	case MaskedEnvMetal:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_SPECULAR_T1);
			mat.PsType = EPST_MAPOBJ_METAL;
		}
		break;
	case EnvMetalEmissive:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_SPECULAR_T1);
			mat.PsType = EPST_MAPOBJ_METAL;
		}
		break;
	case TwoLayerDiffuseOpaque:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1_T2);
			mat.PsType = EPST_MAPOBJ_TWOLAYERDIFFUSEOPAQUE;
		}
		break;
	case TwoLayerDiffuseEmissive:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1_T2);
			mat.PsType = EPST_MAPOBJ_TWOLAYERDIFFUSE;
		}
		break;
	default:
		{
			mat.VertexShader = shaderServices->getVertexShader(EVST_MAPOBJ_DIFFUSE_T1);
			mat.PsType = EPST_MAPOBJ_DIFFUSE;
		}
		break;
	}

	mat.AmbientColor.set(1.0f, 1.0f, 1.0f);
	mat.SpecularColor.set(0.3f, 0.3f, 0.3f);

	if (material->flags & 0x4)
	{
		mat.Cull = ECM_FRONT;
	}

	if(material->flags & 0x10)
	{	
		mat.Lighting = false;
		mat.EmissiveColor = SColorf(material->color0);
	}
	else if (material->flags & 0x20)
	{
		mat.Lighting = false;
		mat.EmissiveColor = SColorf(material->color1);
	}
	else
	{
		mat.Lighting = true;
	}

	mat.FogEnable = EnableFog;

	mat.MaterialType = material->alphatest ? EMT_ALPHA_TEST : EMT_SOLID;

	mat.TextureLayer[0].TextureWrapU = mat.TextureLayer[0].TextureWrapV = ETC_REPEAT;
	mat.TextureLayer[1].TextureWrapU = mat.TextureLayer[1].TextureWrapV = ETC_REPEAT;

}

void CWMOSceneNode::enableFog( bool enable )
{
	EnableFog = enable;
}

void CWMOSceneNode::loadDoodadSceneNodes()
{
	WmoScene->loadDoodadSceneNodes();
}

void CWMOSceneNode::unloadDoodadSceneNodes()
{
	WmoScene->unloadDoodadSceneNodes();
}

void CWMOSceneNode::drawPortal( u32 index, SColor color )
{
	ASSERT(index < Wmo->Header.nPortals);
	const SWMOPortal* portal = &Wmo->Portals[index];
	CFileWMO* fileWmo = static_cast<CFileWMO*>(Wmo);

	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	SRenderUnit unit = {0};

	unit.material.MaterialType = EMT_SOLID;
	unit.material.Lighting = false;
	unit.material.EmissiveColor = color;
	unit.material.Cull = ECM_NONE;
	unit.material.ZWriteEnable = false;

	unit.distance = 0;
	unit.bufferParam.vbuffer0 = fileWmo->PortalVertexBuffer;
	unit.bufferParam.ibuffer = g_Engine->getHardwareBufferServices()->getStaticIndexBufferQuadList();
	unit.bufferParam.vType = EVT_P;
	unit.primType = EPT_TRIANGLES;
	unit.primCount = portal->vCount / 2;			//2 tris per 4 verts

#ifdef USE_WITH_GLES2
	unit.drawParam.voffset0 = portal->vStart;
#else
	unit.drawParam.baseVertIndex = portal->vStart;
#endif
	
	unit.drawParam.startIndex = 0;
	unit.drawParam.minVertIndex = 0;
	unit.drawParam.numVertices = portal->vCount;
	unit.sceneNode = this;
	unit.matWorld = &AbsoluteTransformation;

	sceneRenderServices->addRenderUnit(&unit, ERT_WIRE);	
}

void CWMOSceneNode::drawBspNode( u32 groupIndex, u32 nodeIndex, SColor color )
{
	if (groupIndex >= Wmo->Header.nGroups ||
		nodeIndex >= Wmo->Groups[groupIndex].NumBspNodes ||
		Wmo->Groups[groupIndex].BspNodes[nodeIndex].numfaces == 0)
		return;

	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	const CWMOGroup* group = &Wmo->Groups[groupIndex];
	const SWMOBspNode* node = &group->BspNodes[nodeIndex];

	ASSERT(node->left == -1 && node->right == -1);
	ASSERT(node->firstface + node->numfaces <= (u16)group->NumBspTriangles);

	SRenderUnit unit = {0};

	unit.material.MaterialType = EMT_SOLID;
	unit.material.Lighting = false;
	unit.material.EmissiveColor = color;
	unit.material.Cull = ECM_NONE;
	unit.material.ZWriteEnable = false;

	unit.distance = 0;
	unit.bufferParam.vbuffer0 = group->BspVertexBuffer;
	unit.bufferParam.ibuffer = group->BspIndexbuffer;
	unit.bufferParam.vType = EVT_P;
	unit.primType = EPT_TRIANGLES;
	unit.primCount = node->numfaces;
	unit.drawParam.startIndex = node->startIndex;
	unit.drawParam.minVertIndex = node->minVertex;
	unit.drawParam.numVertices = node->maxVertex - node->minVertex;
	unit.sceneNode = this;
	unit.matWorld = &AbsoluteTransformation;

	sceneRenderServices->addRenderUnit(&unit, ERT_WIRE);	
}

void CWMOSceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame, bool visible )
{
	WmoScene->tick(timeSinceStart, timeSinceLastFrame);
}
