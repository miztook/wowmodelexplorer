#include "stdafx.h"
#include "CEditM2SceneNode.h"
#include "CFileM2.h"
#include "CSceneRenderServices.h"

CEditM2SceneNode::CEditM2SceneNode( IFileM2* mesh, ISceneNode* parent, bool npc )
	: CM2SceneNode(mesh, parent, npc), ShowGeosets(NULL), IsGeosetsVisible(NULL)
{
	ShowModel = true;
	ShowBoundingBox = false;
	ShowBones = false;
	ShowBoundingAABox = false;
	ShowCollisionAABox = false;

	Paused = false;
	TickDelta = 0;

	OverrideWireframe = editor::EOW_NONE;

	CFileSkin* skin = M2Instance->CurrentSkin;
	if (skin)
	{
		ShowGeosets = new bool[skin->NumGeosets];
		for (u32 i=0; i<skin->NumGeosets; ++i)
			ShowGeosets[i] = true;

		IsGeosetsVisible = new bool[skin->NumGeosets];
		::memset(IsGeosetsVisible, 0, sizeof(bool)*skin->NumGeosets);
	}

	BonesDrawn = NULL;
	if (Mesh->NumBones > 0)
	{
		BonesDrawn = new bool[Mesh->NumBones];
		::memset(BonesDrawn, 0, sizeof(bool)*Mesh->NumBones);
	}

	Attachments = new SAttachment[Mesh->NumAttachments];
	for (u32 i=0; i<Mesh->NumAttachments; ++i)
	{
		//meshNode
		IMeshSceneNode* mNode = g_Engine->getSceneManager()->addMeshSceneNode("$sphere8X4", this);
		mNode->setRenderInstType(ERT_WIRE);
		mNode->Visible = false;
		SMaterial& mat = mNode->getMaterial();
		mat.MaterialType = EMT_LINE;
		mat.Wireframe = true;
		mat.ZWriteEnable = false;
		mat.ZBuffer = ECFN_ALWAYS;
		mat.Lighting = false;

		//coordNode
		ICoordSceneNode* cNode = g_Engine->getSceneManager()->addCoordSceneNode(this);
		cNode->Visible = false;

		Attachments[i].attachmentSceneNode = mNode;
		Attachments[i].coordSceneNode = cNode;
		Attachments[i].show = false;
	}
}


CEditM2SceneNode::~CEditM2SceneNode()
{
// 	for (u32 i=0; i<Mesh->NumAttachments; ++i)
// 	{
// 		g_Engine->getSceneManager()->removeSceneNode(Attachments[i].coordSceneNode);
// 	}
	delete[] Attachments;
	delete[] BonesDrawn;
	delete[] IsGeosetsVisible;
	delete[] ShowGeosets;
}

void CEditM2SceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame, bool visible )
{
	if (Paused)
	{
		if (TickDelta)
		{
			CM2SceneNode::tick(timeSinceStart, TickDelta, visible);
			TickDelta = 0;
		}
		return;
	}

	for (T_ParticleSystemNodes::iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		(*i)->setEmitting(true);
	}

	//default
	CM2SceneNode::tick(timeSinceStart, timeSinceLastFrame, visible);

	//
	updateAttachmentSceneNodes();
}

void CEditM2SceneNode::render() const
{
	if (!ShowModel)
		return;

	CFileSkin* skin = M2Instance->CurrentSkin;
	if (skin)
	{
		::memset(IsGeosetsVisible, 0, sizeof(bool)*skin->NumGeosets);

		for (PLENTRY p = M2Instance->VisibleGeosetList.Flink; p != &M2Instance->VisibleGeosetList; p = p->Flink)
		{
			u32 c = (u32)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - M2Instance->DynGeosets);

			if (M2Instance->DynGeosets[c].NoAlpha || !ShowGeosets[c])
				continue;

			IsGeosetsVisible[c] = true;

			renderGeoset(c);
		}
	}

}

void CEditM2SceneNode::renderGeoset( u32 index ) const
{
	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	const CFileSkin* skin = M2Instance->CurrentSkin;

	const CGeoset* set = &M2Instance->CurrentSkin->Geosets[index];
	const SDynGeoset* dset = &M2Instance->DynGeosets[index];

	if (!set->BillBoard)
	{
		for (CGeoset::T_BoneUnits::const_iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
		{
			SRenderUnit unit =  {0};

			if(!M2Instance->setGeosetMaterial(index, unit.material))
				continue;
			unit.material.FogEnable = EnableFog;

			if (RenderInstType == ERT_MESH && ModelAlpha)
				unit.u.priority = RenderPriority + (set->GeoID != 0 ? 1 : 0);	//子模型优先渲染, 身体最后渲染
			unit.distance = Distance;
			unit.u.geoset = (u16)index;

			unit.bufferParam.vbuffer0 = skin->GVertexBuffer; 
			unit.bufferParam.vbuffer1 = skin->AVertexBuffer;
			unit.bufferParam.vType = EVT_PNT2W;
			unit.bufferParam.ibuffer = skin->IndexBuffer;
			unit.primType = EPT_TRIANGLES;
			unit.primCount = itr->TCount;

			unit.drawParam.voffset0 = 0; 
			unit.drawParam.voffset1 = itr->BoneVStart - set->VStart;		//现在位置和顶点位置的偏移
			unit.drawParam.minVertIndex = set->VStart;
			unit.drawParam.startIndex = itr->StartIndex;
			unit.drawParam.numVertices = set->VCount;

			unit.sceneNode = this;
			unit.u.useBoneMatrix = dset->Units[itr->Index].Enable;
			unit.u.boneMatrixArray =  dset->Units[itr->Index].BoneMatrixArray;
			unit.matWorld = &AbsoluteTransformation;

			//texture anim
			unit.material.TextureLayer[0].UseTextureMatrix = dset->UseTextureAnim;
			if (dset->UseTextureAnim)
				unit.material.TextureLayer[0].TextureMatrix = &dset->TextureMatrix;	

			//wireframe
			switch(OverrideWireframe)
			{
			case editor::EOW_NONE:
				unit.material.Wireframe = false;
				break;
			case editor::EOW_WIREFRAME:
				unit.material.Wireframe = true;
				break;
			case editor::EOW_WIREFRAME_SOLID:
				unit.material.Wireframe = true;
				unit.material.MaterialType = EMT_SOLID;
				break;
			case editor::EOW_WIREFRAME_ONECOLOR:
				unit.material.Wireframe = true;
				unit.material.MaterialType = EMT_LINE;
				break;
			}

			//camera anim
			if (CurrentCamera != -1)
			{
				unit.matView = &CurrentView;
				unit.matProjection = &CurrentProjection;
			}

			unit.textures[0] = dset->Texture0;
			unit.textures[1] = dset->Texture1;
			unit.textures[2] = dset->Texture2;

			sceneRenderServices->addRenderUnit(&unit, RenderInstType);
		}
	}
	else
	{
		SRenderUnit unit =  {0};

		if(!M2Instance->setGeosetMaterial(index, unit.material))
			return;
		unit.material.FogEnable = EnableFog;

		if (RenderInstType == ERT_MESH && ModelAlpha)
			unit.u.priority = RenderPriority + (set->GeoID != 0 ? 1 : 0);	//子模型优先渲染, 身体最后渲染
		unit.distance = Distance;
		unit.u.geoset = (u16)index;

		unit.bufferParam = g_Engine->getMeshDecalServices()->BufferParam;
		unit.primType = EPT_TRIANGLES;
		unit.primCount = set->VCount / 2;

		unit.drawParam.baseVertIndex = 0;
		unit.drawParam.startIndex = 0;
		unit.drawParam.numVertices = set->VCount;

		unit.sceneNode = this;
		unit.matWorld = NULL;

		//texture anim
		unit.material.TextureLayer[0].UseTextureMatrix = dset->UseTextureAnim;
		if (dset->UseTextureAnim)
			unit.material.TextureLayer[0].TextureMatrix = &dset->TextureMatrix;	

		//wireframe
		switch(OverrideWireframe)
		{
		case editor::EOW_NONE:
			unit.material.Wireframe = false;
			break;
		case editor::EOW_WIREFRAME:
			unit.material.Wireframe = true;
			break;
		case editor::EOW_WIREFRAME_SOLID:
			unit.material.Wireframe = true;
			unit.material.MaterialType = EMT_SOLID;
			break;
		case editor::EOW_WIREFRAME_ONECOLOR:
			unit.material.Wireframe = true;
			unit.material.MaterialType = EMT_LINE;
			break;
		}

		//camera anim
		if (CurrentCamera != -1)
		{
			unit.matView = &CurrentView;
			unit.matProjection = &CurrentProjection;
		}

		unit.textures[0] = dset->Texture0;
		unit.textures[1] = dset->Texture1;
		unit.textures[2] = dset->Texture2;

		sceneRenderServices->addRenderUnit(&unit, RenderInstType == ERT_MESH ? ERT_MESHDECAL : ERT_DOODADDECAL);
	}
}

void CEditM2SceneNode::showShoulder( bool show )
{
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_SHOULDER)
		{
			ISceneNode* node = static_cast<ISceneNode*>(entry->node);
			node->Visible = show;
		}
	}
}

void CEditM2SceneNode::showLeftHand( bool show )
{
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_HAND_LEFT)
		{
			ISceneNode* node = static_cast<ISceneNode*>(entry->node);
			node->Visible = show;
		}
	}
}

void CEditM2SceneNode::showRightHand( bool show )
{
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_HAND_RIGHT)
		{
			ISceneNode* node = static_cast<ISceneNode*>(entry->node);
			node->Visible = show;
		}
	}
}

void CEditM2SceneNode::showParticles( bool show )
{
	M2Instance->ShowParticles = show;

	for (T_ParticleSystemNodes::iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		(*i)->Visible = show;
	}

	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CEditM2SceneNode* node = static_cast<CEditM2SceneNode*>(entry->node);
		node->showParticles(show);
	}
}

void CEditM2SceneNode::showGeoset( u32 index, bool show )
{
	CFileSkin* skin = M2Instance->CurrentSkin;
	if (!skin || index > skin->NumGeosets)
		return;

	ShowGeosets[index] = show;
}

bool CEditM2SceneNode::isGeosetShow( u32 index )
{
	CFileSkin* skin = M2Instance->CurrentSkin;
	if (!skin || index > skin->NumGeosets)
		return false;

	return ShowGeosets[index];
}

void CEditM2SceneNode::showAttachment( u32 index, bool show )
{
	if (index >= Mesh->NumAttachments)
		return;

	Attachments[index].show = show;
}

bool CEditM2SceneNode::isAttachmentShow( u32 index )
{
	if (index >= Mesh->NumAttachments)
		return false;

	return Attachments[index].show;
}

void CEditM2SceneNode::getGeosetDebugInfo( c16* msg, u32 size )
{
	wcscpy_s((wchar_t*)msg, size, L"Visible Geosets:\n");

	u32 numgeosets = Mesh->Skin->NumGeosets;
	u32 count = 0;
	for (u32 i=0; i<numgeosets; ++i)
	{
		if(IsGeosetsVisible[i])
		{
			++count;

			wchar_t tmp[8] = {0};
			_itow_s(i, tmp, 8, 10);
			wcscat_s((wchar_t*)msg, 512, tmp);
			wcscat_s((wchar_t*)msg, 512, L"  ");

			if (count%5 == 0)
				wcscat_s((wchar_t*)msg, 512, L"\n");
		}
	}
}

void CEditM2SceneNode::drawBoundingBox(SColor color)
{
// 	if (Mesh->NumBoundingVerts && Mesh->NumBoundingTriangles)
// 	{
// 		g_Engine->getDrawServices()->draw3DVerts(Mesh->Bounds, Mesh->NumBoundingVerts,
// 			Mesh->BoundTris, Mesh->NumBoundingTriangles, color, AbsoluteTransformation);
// 	}

// 	if(!Mesh->BoundingAABBox.isZero())
// 		g_Engine->getDrawServices()->add3DBox(Mesh->BoundingAABBox, color);
	g_Engine->getDrawServices()->addAABB(getAnimatedWorldAABB(), color);

	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CEditM2SceneNode* node = reinterpret_cast<CEditM2SceneNode*>(entry->node);
		node->drawBoundingBox(color);
	}
}

void CEditM2SceneNode::drawBoundingAABox( SColor color )
{
	if(!WorldBoundingAABox.isZero())
		g_Engine->getDrawServices()->addAABB(WorldBoundingAABox, color);
}

void CEditM2SceneNode::drawCollisionAABox( SColor color )
{
	if(!WorldCollisionAABox.isZero())
		g_Engine->getDrawServices()->addAABB(WorldCollisionAABox, color);
}

void CEditM2SceneNode::drawBone( u32 idx, SColor color )
{
	s32 parent = Mesh->Bones[idx].parent;
	if (parent == -1 || BonesDrawn[idx])
		return;

	vector3df v0 = M2Instance->DynBones[idx].transPivot;
	vector3df v1 = M2Instance->DynBones[parent].transPivot;
	AbsoluteTransformation.transformVect(v0);
	AbsoluteTransformation.transformVect(v1);

	g_Engine->getDrawServices()->add3DLine(line3df(v0, v1), color);

	BonesDrawn[idx] = true;

	drawBone(parent, color);
}

void CEditM2SceneNode::drawBones( SColor color )
{
	CFileSkin* skin = M2Instance->CurrentSkin;
	if (skin)
	{
		::memset(BonesDrawn, 0, sizeof(bool)*Mesh->NumBones);

		for (PLENTRY p = M2Instance->VisibleGeosetList.Flink; p != &M2Instance->VisibleGeosetList; p = p->Flink)
		{
			u32 c = (u32)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - M2Instance->DynGeosets);

			if (M2Instance->DynGeosets[c].NoAlpha || !ShowGeosets[c])
				continue;

			CGeoset* set = &skin->Geosets[c];
			for (CGeoset::T_BoneUnits::iterator itr=set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
			{
				for(u8 k=0; k<(*itr).BoneCount; ++k)
				{
					u16 idx = (*itr).local2globalMap[k];
					drawBone(idx, color);
				}
			}
		}
	}
			
	//attachments
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CEditM2SceneNode* node = reinterpret_cast<CEditM2SceneNode*>(entry->node);
		node->drawBones(color);
	}
}

void CEditM2SceneNode::updateAttachmentSceneNodes()
{
	//attachment scene nodes
	for (u32 i=0; i<Mesh->NumAttachments; ++i)
	{
		Attachments[i].attachmentSceneNode->Visible = Attachments[i].show;
		Attachments[i].coordSceneNode->Visible = Attachments[i].show;

		const SModelAttachment& attachment = Mesh->Attachments[i];
		s32 idx = Mesh->Attachments[i].boneIndex;
		if (idx == -1)
			continue;

		const SDynBone& b = M2Instance->DynBones[idx];

		{
			matrix4 m;
			m.setScale(0.05f);
			m.setTranslation(attachment.position);
			m = m * b.mat;

			Attachments[i].attachmentSceneNode->setRelativeTransformation(m);
			Attachments[i].attachmentSceneNode->update(false);
		}
		
		{
			matrix4 m;
			m.setScale(0.5f);
			m.setTranslation(attachment.position);
			m = m * b.mat;
		
			Attachments[i].coordSceneNode->setRelativeTransformation(m);
			Attachments[i].coordSceneNode->update(false);
		}
	}
	
}

void CEditM2SceneNode::setOverrideWireframe( editor::E_OVERRIDE_WIREFRAME wireframe )
{
	OverrideWireframe = wireframe;
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CEditM2SceneNode* node = reinterpret_cast<CEditM2SceneNode*>(entry->node);
		node->OverrideWireframe = wireframe;
	}
}

bool CEditM2SceneNode::getChildSceneNodes( editor::SM2ChildSceneNodes* childSceneNodes )
{
	if (!getM2Instance()->CharacterInfo)
		return false;

	memset(childSceneNodes, 0, sizeof(editor::SM2ChildSceneNodes));

	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		IM2SceneNode* node = static_cast<IM2SceneNode*>(entry->node);
		switch(entry->slot)
		{
		case CS_HEAD:
			childSceneNodes->head = node;
			break;
		case CS_HAND_LEFT:
			childSceneNodes->leftHand = node;
			break;
		case CS_HAND_RIGHT:
			childSceneNodes->rightHand = node;
			break;
		case CS_SHOULDER:
			if (entry->id == ATT_LEFT_SHOULDER)
				childSceneNodes->leftShoulder = node;
			else
				childSceneNodes->rightShoulder = node;
			break;
		}
		
	}
	return true;
}

const c8* CEditM2SceneNode::getReplaceTextureFileName( editor::E_MODEL_REPLACE_TEXTURE texture )
{
	ETextureTypes textype = (ETextureTypes)0;

	switch(texture)
	{
	case editor::EMRT_CAPE:
		textype = TEXTURE_CAPE;
		break;
	case editor::EMRT_HAIR:
		textype = TEXTURE_HAIR;
		break;
	case editor::EMRT_BODY:
		textype = TEXTURE_BODY;
		break;
	case editor::EMRT_FUR:
		textype = TEXTURE_FUR;
		break;
	case editor::EMRT_NPC1:
		textype = TEXTURE_GAMEOBJECT1;
		break;
	case editor::EMRT_NPC2:
		textype = TEXTURE_GAMEOBJECT2;
		break;
	case editor::EMRT_NPC3:
		textype = TEXTURE_GAMEOBJECT3;
		break;
	}

	if (textype)
	{
		ITexture* tex = getM2Instance()->ReplaceTextures[textype];
		return tex ? tex->getFileName() : "";
	}
	return "";
}

const c8* CEditM2SceneNode::getTextureFileName( u32 index )
{
	IFileM2* filem2 = getM2Instance()->getMesh();
	if (index >= filem2->NumTextures)
		return "";

	ITexture* tex = filem2->Textures[index];
	return tex ? tex->getFileName() : "";
}

const c8* CEditM2SceneNode::getRegionTextureFileName( ECharRegions region )
{
	if (getM2Instance()->CharacterInfo)
	{
		if(region < NUM_REGIONS)
			return getM2Instance()->CharacterInfo->BodyTextureFileNames[region];
	}
	return "";
}





