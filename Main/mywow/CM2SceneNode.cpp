#include "stdafx.h"
#include "CM2SceneNode.h"
#include "CParticleSystemSceneNode.h"
#include "CRibbonSceneNode.h"
#include "mywow.h"
#include "CFileM2.h"
#include "wow_m2instance.h"
#include "wow_m2appearance.h"
#include "wow_m2FSM.h"
#include "wow_m2Move.h"
#include "wow_m2spell.h"
#include "CSceneRenderServices.h"
#include "CFileM2.h"

CM2SceneNode::CM2SceneNode( IFileM2* mesh, ISceneNode* parent, bool npc )
	: IM2SceneNode(parent), IsNpc(npc),
	CurrentAnim(-1), TimeBlend(0), AnimTimeBlend(0),
	CurrentCamera(-1), AnimateColors(true), Death(false), 
	ModelAlpha(false), EnableFog(false), FrameInterval(0), WorldRadius(0.0f), Scale(1.0f)
{
	Mesh = static_cast<CFileM2*>(mesh);

	Mesh->grab();
	
	M2Instance = new wow_m2instance(Mesh, npc);

	if (!M2Instance->CharacterInfo)			//不允许换装
		M2Appearance = NULL_PTR;
	else
		M2Appearance = new wow_m2appearance(this);

	if (Mesh->getType() == MT_CHARACTER || Mesh->getType() == MT_CREATRUE || Mesh->getType() == MT_INTERFACE)
	{
		M2FSM = new wow_m2FSM(this);
		M2Move = new wow_m2Move(this);
		M2Spell = new wow_m2spell(this);
	}
	else
	{
		M2FSM = NULL_PTR;
		M2Move = NULL_PTR;
		M2Spell = NULL_PTR;
	}

	for (u32 i=0; i<Mesh->NumParticleSystems; ++i)
	{
		IParticleSystemSceneNode* psNode = addParticleSystemSceneNode(&Mesh->ParticleSystems[i]);
		ParticleSystemNodes.push_back(psNode);
	}

	/*
	for (u32 i=0; i<Mesh->NumRibbonEmitters; ++i)
	{
		IRibbonSceneNode* reNode = addRibbonEmitterSceneNode(&Mesh->RibbonEmitters[i]);
		RibbonEmitterNodes.push_back(reNode);
	}
	*/

	setParticleSpeed(1.0f);
}

CM2SceneNode::~CM2SceneNode()
{
	removeMountM2SceneNode();
	removeAllM2Attachments();
	removeAllSpellVisualKits();

	delete M2Spell;
	delete M2Move;
	delete M2FSM;
	delete M2Appearance;
	delete M2Instance;

	Mesh->drop();
}

void CM2SceneNode::registerSceneNode(bool frustumcheck, int sequence)
{
	update(true);

	if (!Visible || WorldRadius == 0)
		return;

	//child nodes register before parent
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	if(cam && !Parent)
		Distance = cam->getPosition().getDistanceFrom(AbsoluteTransformation.getTranslation());
	else
		Distance = Parent ? Parent->Distance : 0.0f;

	for (PLENTRY e = ChildNodeList.Flink; e != &ChildNodeList;)
	{
		ISceneNode* node = reinterpret_cast<ISceneNode*>CONTAINING_RECORD(e, ISceneNode, Link);
		e = e->Flink;

		node->registerSceneNode(frustumcheck, sequence);
	}

	bool visible = true;
	if (frustumcheck && !isNodeEligible())
		visible = false;

	g_Engine->getSceneManager()->registerNodeForRendering(this, visible, sequence);
}

aabbox3df CM2SceneNode::getBoundingBox() const
{
	return Mesh->getBoundingBox();
}

void CM2SceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame, bool visible )
{
	if(visible)
		tickVisible(timeSinceStart, timeSinceLastFrame);
	else
		tickInvisible(timeSinceStart, timeSinceLastFrame);
}

void CM2SceneNode::tickInvisible( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	s32 limit = 100;

	if (!checkFrameLimit(timeSinceLastFrame, limit))
		return;

	bool animEnd = false;
	Animation.buildFrameNumber(timeSinceLastFrame, &animEnd);

	//在最后一帧时触发一次
	if (animEnd)
	{
		if (M2Appearance)
			M2Appearance->onAnimationEnd();

		if(M2FSM)
			M2FSM->onAnimationEnd();
	}

	// fsm
	if (M2FSM)
		M2FSM->tick(timeSinceStart, timeSinceLastFrame);

	if (M2Spell)
		M2Spell->tick(timeSinceStart, timeSinceLastFrame);
}

void CM2SceneNode::tickVisible( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	s32 limit = -1;
	f32 portion = 1.0f;

	f32 distance = Distance / WorldRadius;
	f32 end = g_Engine->getSceneRenderServices()->getM2InvisibleTickDistance();
	f32 begin = g_Engine->getSceneRenderServices()->getM2SlowTickBegin() * end;

	f32 delta = distance - begin;
	if (delta > 0)
	{	
		if (distance < end)
		{
			portion = clamp_(delta / (end - begin), 0.0f, 1.0f);
			limit = (s32)(100 * portion);
		}
		else
		{
			portion = 0.0f;
			limit = 100;
		}
	}

	if (!checkFrameLimit(timeSinceLastFrame, limit))
	{
		return;
	}

	bool animEnd = false;
	f32 deltaFrame = Animation.buildFrameNumber(timeSinceLastFrame, &animEnd);

	//在最后一帧时触发一次
	if (animEnd)
	{
		if (M2Appearance)
			M2Appearance->onAnimationEnd();

		if (M2FSM)
			M2FSM->onAnimationEnd();
	}

	// fsm
	if (M2FSM)
		M2FSM->tick(timeSinceStart, timeSinceLastFrame);

	if (M2Spell)
		M2Spell->tick(timeSinceStart, timeSinceLastFrame);

	u32 lastingFrame = (u32)(timeSinceStart * Animation.getAnimationSpeed());
	lastingFrame %= 38400;

	//
	if (CurrentAnim != -1)
	{
		if ( fabs(deltaFrame) > 0.0f )			//在播放动画，动画运行
		{
			u32 currentFrame = (u32)(Animation.getCurrentFrame() * Animation.getAnimationSpeed());

			f32 blend = 1.0f;
			if (AnimTimeBlend > 0)
			{
				AnimTimeBlend = max_(AnimTimeBlend - (s32)timeSinceLastFrame, 0);
				blend = 1.0f - AnimTimeBlend / (f32)TimeBlend;
			}

			M2Instance->animateColors(CurrentAnim, currentFrame);

			M2Instance->animateBones(CurrentAnim, currentFrame, lastingFrame, blend);

			for (T_ParticleSystemNodes::const_iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
			{
				(*i)->setEmitDensity(portion);

				(*i)->setAnimationFrame(0, currentFrame);

				if (Death && currentFrame > 1000)
					(*i)->setEmitting(false);
				else
					(*i)->setEmitting(true);
			}

			AnimatedWorldAABB = M2Instance->AnimatedBox;
			AbsoluteTransformation.transformBox(AnimatedWorldAABB);
		}
		else
		{
			for (T_ParticleSystemNodes::const_iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
			{
				(*i)->setEmitDensity(portion);

				(*i)->setEmitting(false);
			}
			AnimatedWorldAABB = getWorldBoundingBox();
		}

	}
	else		//无动画, 循环 
	{
		if (AnimateColors)
			M2Instance->animateColors(0, lastingFrame);
		else
			M2Instance->solidColors();

		M2Instance->disableBones();

		for (T_ParticleSystemNodes::const_iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
		{
			(*i)->setEmitDensity(portion);

			(*i)->setAnimationFrame(0, lastingFrame);
			(*i)->setEmitting(true);
		}
		AnimatedWorldAABB = getWorldBoundingBox();
	}

	//anim texture
	M2Instance->animateTextures(0, lastingFrame);

	// 更新Attachment位置
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);
		updateAttachmentEntry(entry);
	}

	for (T_AttachmentList::iterator itr = SpellEffectList.begin(); itr != SpellEffectList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);
		updateSpellEffectEntry(entry);
	}
}

void CM2SceneNode::render() const
{
	CFileSkin* skin = M2Instance->CurrentSkin;
	if (!skin)
		return;

	for (PLENTRY p = M2Instance->VisibleGeosetList.Flink; p != &M2Instance->VisibleGeosetList;)
	{
		u32 c = (u32)(reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link) - M2Instance->DynGeosets);
		p = p->Flink;
		
		if (M2Instance->DynGeosets[c].NoAlpha)
			continue;

		renderGeoset(c);
	}

	//aabbox3df box = getAnimatedWorldAABB(); //Mesh->BoundingAABBox;
	//AbsoluteTransformation.transformBox(box);
	//g_Engine->getDrawServices()->add3DBox(box, SColor(255,0,0));

  	//g_Engine->getDrawServices()->draw2DImage(M2Instance->ReplaceTextures[TEXTURE_BODY], vector2di(300,0), NULL_PTR, SColor(), 1.0f, false);
}

void CM2SceneNode::renderGeoset( u32 index ) const
{
	CSceneRenderServices* sceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());

	const CFileSkin* skin = M2Instance->CurrentSkin;

	const CGeoset* set = &skin->Geosets[index];

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
		unit.material.AmbientColor.set(1.0f, 1.0f, 1.0f, 1.0f);
		unit.material.EmissiveColor.set(0.0f, 0.0f, 0.0f, 0.0f);

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
		unit.matWorld = NULL_PTR;

		//texture anim
		unit.material.TextureLayer[0].UseTextureMatrix = dset->UseTextureAnim;
		if (dset->UseTextureAnim)
			unit.material.TextureLayer[0].TextureMatrix = &dset->TextureMatrix;	

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

u32 CM2SceneNode::onFillVertexBuffer( u32 geoset, SVertex_PCT* vertices, u32 vcount ) const
{
	CFileSkin* skin = M2Instance->CurrentSkin;
	CGeoset* set = &skin->Geosets[geoset];
	ASSERT(set->BillboardRects);

	SColor color = M2Instance->DynGeosets[geoset].finalColor;

	ICamera* camera = g_Engine->getSceneManager()->getActiveCamera();

	const matrix4& view = CurrentCamera != -1 ? CurrentView : camera->getViewMatrix();

	vector3df right( view[0], view[4], view[8] );
	vector3df up( view[1], view[5], view[9] );
	vector3df dir( view[2], view[6], view[10]);

	u32 vCount = 0;

	for (u32 i=0; i<(u32)(set->VCount/4); ++i)
	{
		if (vCount + 4 > vcount)
			break;

		SBRect* rect = &set->BillboardRects[i];
		vector3df center = rect->center;
		matrix4 mat = M2Instance->DynBones[rect->boneIndex].mat * AbsoluteTransformation;
		mat.transformVect(center);

 		vector3df w = right * rect->width * Scale;
 		vector3df h= up * rect->height * Scale;

		vertices[vCount + 0].Pos = center - w + h;
		vertices[vCount + 1].Pos = center +  w + h;
		vertices[vCount + 2].Pos = center - w - h;
		vertices[vCount + 3].Pos = center +  w - h;

		vertices[vCount + 0].TCoords = rect->texcoords[0];
		vertices[vCount + 1].TCoords = rect->texcoords[1];
		vertices[vCount + 2].TCoords = rect->texcoords[2];
		vertices[vCount + 3].TCoords = rect->texcoords[3];

		vertices[vCount + 0].Color =
		vertices[vCount + 1].Color = 
		vertices[vCount + 2].Color = 
		vertices[vCount + 3].Color = color;

		vCount += 4;
	}

	return vCount;
}

bool CM2SceneNode::isNodeEligible() const
{
	aabbox3df box = getAnimatedWorldAABB();
	if (box.isZero())
		return false;

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	return cam->getViewFrustum().isInFrustum(box);
}

bool CM2SceneNode::playAnimationByIndex( u32 anim, bool loop, s32 timeblend /*= 200*/ )
{
	if (Mesh->NumAnimations == 0)
	{
		CurrentAnim = -1;
		return false;
	}

	anim = anim % Mesh->NumAnimations;
	SModelAnimation* a = &Mesh->Animations[anim];

	Animation.setMaxFrame((s32)a->timeLength);
	Animation.setFrameLoop( 0, (s32)a->timeLength);
	Animation.setLoopMode(loop);
	Animation.setCurrentFrame(0);

	CurrentAnim = (s32)anim;
	AnimTimeBlend = TimeBlend = timeblend;

	if (M2Instance->CharacterInfo)
		M2Instance->CharacterInfo->Blink = a->animID == 0;

	Death = a->animID == 1;

	return true;
}

bool CM2SceneNode::playAnimationByName( const c8* name, u32 subIndx, bool loop, s32 timeblend /*= 200*/ )
{
	s16 idx = Mesh->getAnimationIndex(name, subIndx);
	if(idx == -1)
	{
		CurrentAnim = -1;
		return false;
	}

	return playAnimationByIndex((u32)idx, loop, timeblend);
}

void CM2SceneNode::setM2ModelEquipment( s32 slot, s32 itemid, bool sheath )
{
	removeM2ModelEquipment(slot);

	if (itemid == 0)
	{
		if(slot == CS_HAND_LEFT)
			updateLeftCloseHand(false);
		else if (slot == CS_HAND_RIGHT)
			updateRightCloseHand(false);

		return;
	}

	SAttachmentEntry entry1;
	SAttachmentEntry entry2;
	SAttachmentInfo info1, info2;
	M2Instance->setM2Equipment(slot, itemid, sheath, &entry1, &info1, &entry2, &info2);

	IFileM2* m1 = NULL_PTR;
	if (entry1.attachIndex != -1 && strcmp(info1.modelpath, "") != 0)
	{
		m1 = g_Engine->getResourceLoader()->loadM2(info1.modelpath);
		if (m1)
		{
			M2Instance->UseAttachments[entry1.attachIndex] += 1;

			IM2SceneNode* node = g_Engine->getSceneManager()->addM2SceneNode(m1, this);
			wow_m2instance* c = node->getM2Instance();
			ITexture* t = g_Engine->getResourceLoader()->loadTexture(info1.texpath);
			c->setItemTexture(t);
			c->buildVisibleGeosets();
			if (entry1.slot == CS_SHOULDER || entry1.slot == CS_HAND_RIGHT || entry1.slot == CS_HAND_LEFT /*|| entry1->slot == CS_HEAD*/)
			{
				node->playAnimationByName("Stand", 0, true);	
			}
			entry1.node = node;
			AttachmentList.push_back(entry1);

			updateAttachmentEntry(&entry1);

			if (entry1.slot == CS_HEAD)
			{
				if (M2Appearance)
				{
					M2Appearance->HelmNode = node;
					M2Appearance->HideHelmHair = M2Instance->CharacterInfo->HelmHideHair;
					M2Appearance->HideHelmFacial1 = M2Instance->CharacterInfo->HelmHideFacial1;
					M2Appearance->HideHelmFacial2 = M2Instance->CharacterInfo->HelmHideFacial2;
					M2Appearance->HideHelmFacial3 = M2Instance->CharacterInfo->HelmHideFacial3;
				}
			}
		}
	}

	IFileM2* m2 = NULL_PTR;
	if (entry2.attachIndex != -1 && strcmp(info2.modelpath, "") != 0)
	{
		m2 = g_Engine->getResourceLoader()->loadM2(info2.modelpath);
		if (m2)
		{		
			M2Instance->UseAttachments[entry2.attachIndex] += 1;
			
			IM2SceneNode* node = g_Engine->getSceneManager()->addM2SceneNode(m2, this);
			wow_m2instance* c = node->getM2Instance();
			ITexture* t = g_Engine->getResourceLoader()->loadTexture(info2.texpath);
			c->setItemTexture(t);
			c->buildVisibleGeosets();
			if (entry2.slot == CS_SHOULDER || entry2.slot == CS_HAND_LEFT || entry2.slot == CS_HAND_RIGHT)
			{
				node->playAnimationByName("Stand", 0, true);	
			}
			entry2.node = node;
			AttachmentList.emplace_back(entry2);

			updateAttachmentEntry(&entry2);
		}
	}

	if(slot == CS_HAND_LEFT)
		updateLeftCloseHand(M2Appearance->isLeftWeaponSheathed());
	else if (slot == CS_HAND_RIGHT)
		updateRightCloseHand(M2Appearance->isRightWeaponSheathed());
	
	setModelAlpha(M2Instance->EnableModelAlpha, M2Instance->ModelAlpha);
	setModelColor(M2Instance->EnableModelColor, M2Instance->ModelColor);
}

bool CM2SceneNode::setMountM2SceneNode( IM2SceneNode* m2Node )
{
	bool ret = removeMountM2SceneNode();

	if (m2Node)
	{
		SAttachmentEntry entry;
		M2Instance->setM2Mount(&entry);
		if (entry.attachIndex != -1)
		{
			M2Instance->UseAttachments[entry.attachIndex] += 1;

			entry.node = m2Node;
			AttachmentList.emplace_back(entry);

			g_Engine->getSceneManager()->removeSceneNode(m2Node, false);

			addChild(m2Node);
			updateAttachmentEntry(&entry);

			ret = true;
		}
		else
		{
			ret = false;
		}
	}

	return ret;
}

void CM2SceneNode::setSpellVisualKit(u32 spellvkId)
{
	removeAllSpellVisualKits();
	if (spellvkId == 0)		
		return;

	SSpellVKInfo vkinfo;
	M2Instance->setSpellVisualKit(spellvkId, &vkinfo);

	for (u32 i=0; i<SVK_COUNT; ++i)
	{
		SAttachmentEntry* entry = &vkinfo.attachmentEntries[i];
		SAttachmentInfo* info = &vkinfo.attachmentInfos[i];

		IM2SceneNode* node = NULL_PTR;

		if (entry->attachIndex != -1)
		{
			M2Instance->UseAttachments[entry->attachIndex] += 1;

			IFileM2* m2 = g_Engine->getResourceLoader()->loadM2(info->modelpath);
			if (m2)
			{
				node = g_Engine->getSceneManager()->addM2SceneNode(m2, NULL_PTR, false);
				wow_m2instance* c = node->getM2Instance();
				c->buildVisibleGeosets();
				entry->node = node;
				SpellEffectList.emplace_back(*entry);

				updateSpellEffectEntry(entry);	
			}
		}
		M2Spell->DynSpell.nodes[i] = node;
	}
}

void CM2SceneNode::removeM2ModelEquipment( s32 slot )
{
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end();)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == slot)
		{		
			M2Instance->UseAttachments[entry->attachIndex] -= 1;

			ISceneNode* n = reinterpret_cast<ISceneNode*>(entry->node);
			if (removeChild(n))
			{
				n->removeAllChildren();
				delete n;
			}
			
			if (slot == CS_HEAD)
			{
				M2Instance->CharacterInfo->HelmHideHair = false;
				
				if (M2Appearance)
				{
					M2Appearance->HelmNode = NULL_PTR;
					M2Appearance->HideHelmHair = false;
					M2Appearance->HideHelmFacial1 = false;
					M2Appearance->HideHelmFacial2 = false;
					M2Appearance->HideHelmFacial3 = false;
				}
			}

			itr = AttachmentList.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

bool CM2SceneNode::removeMountM2SceneNode()
{
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end();)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot == CS_MOUNT && entry->id == ATT_MOUNT_POINT)
		{		
			M2Instance->UseAttachments[entry->attachIndex] -= 1;

			ISceneNode* n = reinterpret_cast<ISceneNode*>(entry->node);
			removeChild(n);
			
			g_Engine->getSceneManager()->addSceneNode(n);

			itr = AttachmentList.erase(itr);

			return true;
		}
		else
		{
			++itr;
		}
	}
	return false;
}

void CM2SceneNode::removeSpellVisualKit(IM2SceneNode* node)
{
	for (T_AttachmentList::iterator itr = SpellEffectList.begin(); itr != SpellEffectList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->node == node)
		{		
			M2Instance->UseAttachments[entry->attachIndex] -= 1;

			u32 idx = (u32)(entry->slot - CS_EFFECT_HEAD);
			M2Spell->DynSpell.nodes[idx] = NULL_PTR;

			IM2SceneNode* n = reinterpret_cast<IM2SceneNode*>(entry->node);
			g_Engine->getSceneManager()->removeSceneNode(n, true);

			SpellEffectList.erase(itr);

			break;
		}
	}
}

void CM2SceneNode::removeAllSpellVisualKits()
{
	for (T_AttachmentList::iterator itr = SpellEffectList.begin(); itr != SpellEffectList.end();)
	{
		SAttachmentEntry* entry = &(*itr);

		if (entry->slot >= CS_EFFECT_HEAD && entry->slot <= CS_EFFECT_AOE)
		{		
			M2Instance->UseAttachments[entry->attachIndex] -= 1;

			u32 idx = (u32)(entry->slot - CS_EFFECT_HEAD);
			M2Spell->DynSpell.nodes[idx] = NULL_PTR;

			IM2SceneNode* n = reinterpret_cast<IM2SceneNode*>(entry->node);
			g_Engine->getSceneManager()->removeSceneNode(n, true);

			itr = SpellEffectList.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void CM2SceneNode::removeAllM2Attachments()
{
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);
		if (entry->slot == CS_MOUNT)				//跳过坐骑
			continue;

		ISceneNode* node = reinterpret_cast<ISceneNode*>(entry->node);
		if (removeChild(node))
		{
			node->removeAllChildren();
			delete node;
		}
	}
	AttachmentList.clear();
}

void CM2SceneNode::setParticleSpeed(float speed)
{
	f32 particleSpeed = Animation.getAnimationSpeed() * speed;
	for (T_ParticleSystemNodes::const_iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		(*i)->setSpeed(particleSpeed);
	}
	ParticleSpeed = speed;
}

void CM2SceneNode::updateAttachmentEntry( const SAttachmentEntry* entry )
{
	const SModelAttachment& attachment = Mesh->Attachments[entry->attachIndex];
	s32 bIdx = attachment.boneIndex;
	if(bIdx == -1)
		return;

	const SDynBone& b = M2Instance->DynBones[bIdx];
	ISceneNode* node = reinterpret_cast<ISceneNode*>(entry->node);

	matrix4 m;
	if (entry->scale != 1.0f)
		m.setScale(entry->scale);
	m.setTranslation(attachment.position);
	m = m * b.mat;

	node->setRelativeTransformation(m);
	node->update(true);
}

void CM2SceneNode::updateSpellEffectEntry( const SAttachmentEntry* entry )
{
	const SModelAttachment& attachment = Mesh->Attachments[entry->attachIndex];
	s32 bIdx = attachment.boneIndex;
	if(bIdx == -1)
		return;

	const SDynBone& b = M2Instance->DynBones[bIdx];
	IM2SceneNode* node = reinterpret_cast<IM2SceneNode*>(entry->node);

	matrix4 m;
	if (entry->scale != 1.0f)
		m.setScale(entry->scale);
	m.setTranslation(attachment.position);
	m = m * b.mat * AbsoluteTransformation;

	vector3df dst = -g_Engine->getSceneManager()->getActiveCamera()->getDir();
	dst.Y = 0.0f;
	dst.normalize();
	quaternion q;
	q.rotationFromTo(vector3df::UnitX(), dst, vector3df::UnitY());

	matrix4 mat;
	q.getMatrix(mat);
	mat.setScale(m.getScale());
	mat.setTranslation(m.getTranslation());

	node->setRelativeTransformation(mat);
	node->update(true);
}

IParticleSystemSceneNode* CM2SceneNode::addParticleSystemSceneNode( ParticleSystem* ps )
{
	if (!ps)
		return NULL_PTR;

	return new CParticleSystemSceneNode(ps, this);
}

IRibbonSceneNode* CM2SceneNode::addRibbonEmitterSceneNode( RibbonEmitter* re )
{
	if (!re)
		return NULL_PTR;

	return new CRibbonSceneNode(re, this);
}

void CM2SceneNode::updateCharacter()
{
	if (M2Instance->CharacterInfo && !IsNpc)
	{
		M2Instance->updateCharacter();
	}
}

void CM2SceneNode::buildVisibleGeosets()
{
	M2Instance->buildVisibleGeosets();
}

bool CM2SceneNode::updateNpc( s32 npcid )
{
	if (M2Instance->CharacterInfo && IsNpc)
	{
		bool success = M2Instance->updateNpc(npcid);
		if (success)
		{
			for (u32 slot=0; slot<NUM_CHAR_SLOTS; ++slot)
			{
				s32 id = M2Instance->CharacterInfo->Equipments[slot];
				if (M2Instance->slotHasModel(slot))
					setM2ModelEquipment(slot, id, false);
			}
			M2Instance->updateCharacter();
		}
		return success;
	}
	else if(Mesh->getType() == MT_CREATRUE)
	{
		return M2Instance->updateNpc(npcid);
	}

	return false;
}

bool CM2SceneNode::setModelCamera( s32 index )
{
	if (index < 0 || index >= (s32)Mesh->NumModelCameras)
	{
		CurrentCamera = -1;

		for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
		{
			SAttachmentEntry* entry = &(*itr);

			CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
			node->CurrentCamera = -1;
		}

		for (T_AttachmentList::iterator itr = SpellEffectList.begin(); itr != SpellEffectList.end(); ++itr)
		{
			SAttachmentEntry* entry = &(*itr);

			CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
			node->CurrentCamera = -1;
		}

		for (T_ParticleSystemNodes::const_iterator i = ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
		{
			IParticleSystemSceneNode* node = (IParticleSystemSceneNode*)(*i);
			node->setOwnView(NULL_PTR);
			node->setOwnProjection(NULL_PTR);
		}

		return false;
	}

	CurrentCamera = index;

	SModelCamera& cam = Mesh->ModelCameras[index];
	f32 nearValue = cam.nearclip;
	f32 farValue = cam.farclip;
	//recti viewport = g_Engine->getDriver()->getViewPort();
	f32 aspectRatio = 1.6667f; //(f32)viewport.getWidth() / (f32)viewport.getHeight();

	CurrentProjection.buildProjectionMatrixPerspectiveFovLH(PI/4.0f, aspectRatio, nearValue, farValue);
	vector3df pos = cam.position;
	vector3df target = cam.target;

	CurrentView.buildCameraLookAtMatrixLH(pos, target, vector3df(0,1,0));

	//attachments
	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
		node->CurrentCamera = index;
		node->CurrentProjection = CurrentProjection;
		node->CurrentView = CurrentView;
	}

	//spelleffects
	for (T_AttachmentList::iterator itr = SpellEffectList.begin(); itr != SpellEffectList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
		node->CurrentCamera = index;
		node->CurrentProjection = CurrentProjection;
		node->CurrentView = CurrentView;
	}

	//particles
	for (T_ParticleSystemNodes::const_iterator i = ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		IParticleSystemSceneNode* node = (IParticleSystemSceneNode*)(*i);
		node->setOwnView(&CurrentView);
		node->setOwnProjection(&CurrentProjection);
	}

	return true;
}

void CM2SceneNode::updateLeftCloseHand( bool sheath )
{
	if (sheath)
		M2Instance->CharacterInfo->CloseLHand = false;
	else
		M2Instance->CharacterInfo->CloseLHand = M2Instance->CharacterInfo->Equipments[CS_HAND_LEFT] != 0;
}

void CM2SceneNode::updateRightCloseHand( bool sheath )
{
	if (sheath)
		M2Instance->CharacterInfo->CloseRHand = false;
	else
		M2Instance->CharacterInfo->CloseRHand = M2Instance->CharacterInfo->Equipments[CS_HAND_RIGHT] != 0;	
}

void CM2SceneNode::onUpdated()
{
	IM2SceneNode::onUpdated();

	if (CurrentAnim != -1)
	{
		if (M2Instance->AnimatedBox.isZero())
			AnimatedWorldAABB = getWorldBoundingBox();
		else
		{
			AnimatedWorldAABB = M2Instance->AnimatedBox;
			AbsoluteTransformation.transformBox(AnimatedWorldAABB);
		}
	}
	else
	{
		AnimatedWorldAABB = getWorldBoundingBox();
	}

	vector3df ext = getWorldBoundingBox().getExtent();
	WorldRadius = (ext.X + ext.Y + ext.Z) / 3.0f;

	Scale = AbsoluteTransformation.getScale().X;

	WorldBoundingAABox = Mesh->BoundingAABBox;
	if (!WorldBoundingAABox.isZero())
		AbsoluteTransformation.transformBox(WorldBoundingAABox);

	WorldCollisionAABox = Mesh->CollisionAABBox;
	if (!WorldCollisionAABox.isZero())
		AbsoluteTransformation.transformBox(WorldCollisionAABox);
}

void CM2SceneNode::setModelAlpha( bool enable, f32 val )
{
	f32 v = clamp_(val, 0.0f, 1.0f);
	M2Instance->EnableModelAlpha = enable;
	M2Instance->ModelAlpha = v;

	//model的alpha状态
	ModelAlpha = enable && v < 1.0f;

	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
		node->setModelAlpha(enable, v);
		switch(entry->slot)
		{
		case CS_HEAD:
		case CS_SHOULDER:
			node->RenderPriority = 2;
			break;
		case CS_HAND_LEFT:
		case CS_HAND_RIGHT:
			node->RenderPriority = -2;
			break;
		default:
			break;
		}
	}

	for (T_AttachmentList::iterator itr = SpellEffectList.begin(); itr != SpellEffectList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
		node->setModelAlpha(enable, v);
	}

	for (T_ParticleSystemNodes::const_iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		(*i)->setWholeAlpha(enable,v);
	}
}

void CM2SceneNode::setModelColor( bool enable, SColor color )
{
	M2Instance->EnableModelColor = enable;
	M2Instance->ModelColor = color;

	for (T_AttachmentList::iterator itr = AttachmentList.begin(); itr != AttachmentList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
		node->setModelColor(enable, color);
	}

	for (T_AttachmentList::iterator itr = SpellEffectList.begin(); itr != SpellEffectList.end(); ++itr)
	{
		SAttachmentEntry* entry = &(*itr);

		CM2SceneNode* node = reinterpret_cast<CM2SceneNode*>(entry->node);
		node->setModelColor(enable, color);
	}

	for (T_ParticleSystemNodes::const_iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		(*i)->setWholeColor(enable,color);
	}
}

bool CM2SceneNode::checkFrameLimit( u32& timeSinceLastFrame, s32 limit )
{
	if (limit <= -1)
		return true;

	FrameInterval += timeSinceLastFrame;

	if (FrameInterval < (u32)limit)		//use limit
	{
		return false;
	}
	else
	{
		timeSinceLastFrame = FrameInterval;
		FrameInterval = 0;

		return true;
	}
}
