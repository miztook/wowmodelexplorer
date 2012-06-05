#include "stdafx.h"
#include "CM2SceneNode.h"
#include "CParticleSystemSceneNode.h"
#include "CRibbonSceneNode.h"
#include "mywow.h"
#include "CFileM2.h"

CM2SceneNode::CM2SceneNode( IFileM2* mesh, ISceneNode* parent, bool npc )
	: Mesh(mesh), IM2SceneNode(parent), IsNpc(npc),
	CurrentAnim(-1), TimeBlend(0), AnimTimeBlend(0), Restart(true),
	CurrentCamera(-1), AnimateColors(true), Death(false)
{
	Mesh->grab();

	Character = new wow_character(Mesh, npc);

	HelmNode = NULL;
	OwnProjection = NULL;

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
	removeAllM2ModelEquipments();

	delete Character;

	Mesh->drop();
}

void CM2SceneNode::registerSceneNode(bool frustumcheck)
{
	update(true);

	if (!Visible)
		return;

	if (frustumcheck && !isNodeEligible())
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this);

	IM2SceneNode::registerSceneNode(frustumcheck);
}

aabbox3df CM2SceneNode::getBoundingBox() const
{
	aabbox3df box = Mesh->getBoundingBox();
	//Character->DynBones[1].mat.transformBox(box);
	return box;
}

void CM2SceneNode::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	f32 deltaFrame = Animation.buildFrameNumber(timeSinceLastFrame);

	if (HelmNode && deltaFrame < 0)
	{
		if (rand_() % 3 == 0)
			HelmNode->AnimateColors = false;
		else 
			HelmNode->AnimateColors = true;
	}

	u32 lastingFrame = (u32)(timeSinceStart * Animation.getAnimationSpeed());

	//
	if (CurrentAnim != -1)
	{
		if ( fabs(deltaFrame) >= 1.0f )			//在播放动画，动画运行
		{
			u32 currentFrame = (u32)(Animation.getCurrentFrame() * Animation.getAnimationSpeed());

			f32 blend = 0.8f;
			if (AnimTimeBlend > 0)
			{
				AnimTimeBlend -= (s32)timeSinceLastFrame;
				blend = 0.8f * (1-AnimTimeBlend / (f32)TimeBlend);
			}

			Character->animateColors(CurrentAnim, currentFrame);

			if (Mesh->ContainsBillboardBone)
			{
				matrix4 rot;
				getBillboardBoneMat(rot);
				Character->animateBones(CurrentAnim, currentFrame, lastingFrame, Restart ? blend : 1.0f, &rot);
			}
			else
			{
				Character->animateBones(CurrentAnim, currentFrame, lastingFrame, Restart ? blend : 1.0f, NULL);
			}

			Restart = false;

			for (T_ParticleSystemNodes::iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
			{
				(*i)->setAnimationFrame(0, currentFrame);

				if (Death && currentFrame > 1000)
					(*i)->setEmitting(false);
				else
					(*i)->setEmitting(true);
			}
		}
		else
		{
			for (T_ParticleSystemNodes::iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
			{
				(*i)->setEmitting(false);
			}
		}

//		updateAABB();
	}
	else		//无动画, 循环 
	{
		if (AnimateColors)
			Character->animateColors(0, lastingFrame);
		else
			Character->solidColors();

		if (Mesh->ContainsBillboardBone)
		{
			matrix4 rot;
			getBillboardBoneMat(rot);
			Character->animateBones(&rot);
		}
		else
		{
			Character->disableBones();
		}

		for (T_ParticleSystemNodes::iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
		{
			(*i)->setAnimationFrame(0, lastingFrame);
			(*i)->setEmitting(true);
		}
	}

	//anim texture
	Character->animateTextures(0, lastingFrame);

	// 更新Attachment位置
	for(T_AttachmentEntries::iterator i=AttachmentEntries.begin(); i != AttachmentEntries.end(); ++i)
	{
		updateAttachmentEntry((*i));
	}
}

void CM2SceneNode::render()
{
	CFileSkin* skin = Character->CurrentSkin;
	if (!skin)
		return;

	for (PLENTRY p = Character->VisibleGeosetList.Flink; p != &Character->VisibleGeosetList; p = p->Flink)
	{
		u32 c = reinterpret_cast<SDynGeoset*>CONTAINING_RECORD(p, SDynGeoset, Link)->Index;

		if (Character->DynGeosets[c].NoAlpha)
			continue;

		renderGeoset(c);
	}

	//aabbox3df box = getWorldBoundingBox(); //Mesh->BoundingAABBox;
	//AbsoluteTransformation.transformBox(box);
	//g_Engine->getDrawServices()->add3DBox(box, SColor(255,0,0));

// 	if (Character->getType() == MT_ITEM)
// 		g_Engine->getDrawServices()->draw2DImage(Character->ReplaceTextures[TEXTURE_ITEM], vector2di(0,0), true);
}

void CM2SceneNode::renderGeoset( u32 index )
{
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	CFileSkin* skin = Character->CurrentSkin;

	CGeoset* set = &Character->CurrentSkin->Geosets[index];
	SDynGeoset* dset = &Character->DynGeosets[index];
	for (CGeoset::T_BoneUnitList::iterator itr =set->BoneUnits.begin(); itr != set->BoneUnits.end(); ++itr)
	{
		SRenderUnit unit =  {0};

		if(!Character->setGeosetMaterial(index, unit.material))
			continue;

		unit.vbuffer = skin->VertexBuffer; 
		unit.vbuffer2 = skin->BoneBuffer;
		unit.ibuffer = skin->IndexBuffer;
		unit.primType = EPT_TRIANGLES;
		unit.primCount = itr->PrimCount;

		unit.drawParam.voffset = 0; 
		unit.drawParam.voffset2 = itr->BoneVStart - set->VStart;		//现在位置和顶点位置的偏移
		unit.drawParam.startIndex = itr->StartIndex;
		unit.drawParam.numVertices = set->VCount;
		
		unit.sceneNode = this;
		unit.u.useBoneMatrix = dset->Units[itr->Index].Enable;
		unit.u.boneMatrixArray =  dset->Units[itr->Index].BoneMatrixArray;
		unit.matWorld = &AbsoluteTransformation;
 
		//texture anim
		unit.material.TextureLayer[0].UseTextureMatrix = dset->UseTextureAnim;
		if (dset->UseTextureAnim)
		{
			unit.material.TextureLayer[0].TextureMatrix = &dset->TextureMatrix;	
		}

		//camera anim
		if (CurrentCamera != -1)
		{
			unit.matView = &CurrentView;
			unit.matProjection = &CurrentProjection;
		}
		else
		{
			unit.matProjection = OwnProjection;
		}

		unit.textures[0] = dset->Textures[0];

		//alpha
		if (unit.material.getMaterialAlpha() < 1.0f && !unit.material.isTransparent())
		{
			unit.material.MaterialType = EMT_TRANSPARENT_ALPHA_BLEND;
		}

		 g_Engine->getSceneRenderServices()->addRenderUnit(&unit, RenderInstType);
	}
}

bool CM2SceneNode::isNodeEligible()
{
	aabbox3df box = getWorldBoundingBox();
	if (box.isZero())
		return false;

	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	if (!cam)
		return false;
	return cam->getViewFrustum().isInFrustum(box);
}

bool CM2SceneNode::playAnimationByIndex( u32 anim, bool loop, f32 speed /*= 1.0f*/, s32 timeblend /*= 200*/, bool restart /*= true*/ )
{
	if (Mesh->NumAnimations == 0)
	{
		CurrentAnim = -1;

	//	Character->disableBones();

		return false;
	}

	anim = anim % Mesh->NumAnimations;
	SModelAnimation* a = &Mesh->Animations[anim];

	Animation.setMaxFrame((s32)a->timeLength);
	Animation.setFrameLoop( 0, (s32)a->timeLength);
	Animation.setLoopMode(loop);
	Animation.setAnimationSpeed(speed);
	Animation.setCurrentFrame(0);

	CurrentAnim = (s32)anim;
	AnimTimeBlend = TimeBlend = timeblend;
	Restart = restart;

	Character->Blink = a->animID == 0;

	Death = a->animID == 1;

	return true;
}

bool CM2SceneNode::playAnimationByName( const c8* name, u32 subIndx, bool loop, f32 speed /*= 1.0f*/, s32 timeblend /*= 200*/, bool restart /*= true*/ )
{
	s16 idx = Mesh->getAnimationIndex(name, subIndx);
	if(idx == -1)
	{
		CurrentAnim = -1;
		return false;
	}

	CurrentAnimName = name;

	return playAnimationByIndex((u32)idx, loop, speed, timeblend, restart);
}

bool CM2SceneNode::takeItem( s32 itemid, s32* itemslot )
{
	if (Mesh->getType() != MT_CHARACTER || IsNpc)
		return false;

	s32 slot = Character->getItemSlot(itemid);
	if (slot == -1)
		return false;

	Character->updateEquipments(slot, itemid);

	if (Character->slotHasModel(slot))
		setM2ModelEquipment(slot, itemid);

	if (itemslot)
		*itemslot = slot;

	return true;
}

void CM2SceneNode::loadStartOutfit( s32 startid, bool deathknight )
{
 	Character->dressStartOutfit(startid);

	for (u32 slot=0; slot<NUM_CHAR_SLOTS; ++slot)
	{
		s32 id = Character->Equipments[slot];
		if (Character->slotHasModel(slot))
			setM2ModelEquipment(slot, id);
	}

	Character->DeathKnight = deathknight;
}

void CM2SceneNode::loadSet( s32 setid )
{
	Character->dressSet(setid);
	for (u32 slot=0; slot<NUM_CHAR_SLOTS; ++slot)
	{
		s32 id = Character->Equipments[slot];
		if (Character->slotHasModel(slot))
			setM2ModelEquipment(slot, id);
	}
}


void CM2SceneNode::setM2ModelEquipment( s32 slot, s32 itemid )
{
	removeM2ModelEquipment(slot);

	if (itemid == 0)
	{
		updateCloseHands();
		return;
	}

	SAttachmentEntry *entry1 = new SAttachmentEntry;
	SAttachmentEntry* entry2 = new SAttachmentEntry;
	Character->setM2Equipment(slot, itemid, entry1, entry2);

	IFileM2* m1 = NULL;
	if (entry1->attachIndex != -1 )
	{
		m1 = g_Engine->getResourceLoader()->loadM2(entry1->modelpath);
		if (m1)
		{
			Character->ShowAttachments[entry1->attachIndex] = true;

			IM2SceneNode* node = g_Engine->getSceneManager()->addM2SceneNode(m1, this);
			wow_character* c = node->getWowCharacter();
			ITexture* t = g_Engine->getResourceLoader()->loadTexture(entry1->texpath);
			c->setItemTexture(t);
			c->buildVisibleGeosets();
			entry1->node = node;
			AttachmentEntries.push_back(entry1);

			updateAttachmentEntry(entry1);

			if (entry1->slot == CS_HEAD)
			{
				HelmNode = (CM2SceneNode*)node;
				HideHelmHair = Character->HelmHideHair;
			}
		}
	}

	if (!m1)
		delete entry1;

	IFileM2* m2 = NULL;
	if (entry2->attachIndex != -1)
	{
		Character->ShowAttachments[entry2->attachIndex] = true;
		
		m2 = g_Engine->getResourceLoader()->loadM2(entry2->modelpath);
		if (m2)
		{
			IM2SceneNode* node = g_Engine->getSceneManager()->addM2SceneNode(m2, this);
			wow_character* c = node->getWowCharacter();
			ITexture* t = g_Engine->getResourceLoader()->loadTexture(entry2->texpath);
			c->setItemTexture(t);
			c->buildVisibleGeosets();
			entry2->node = node;
			AttachmentEntries.push_back(entry2);

			updateAttachmentEntry(entry2);
		}
	}

	if (!m2)
		delete entry2;

	updateCloseHands();
}

void CM2SceneNode::removeM2ModelEquipment( s32 slot )
{
	for (T_AttachmentEntries::iterator i=AttachmentEntries.begin(); i != AttachmentEntries.end();)
	{
		SAttachmentEntry* entry = (*i);
		if (entry->slot == slot)
		{		
			Character->ShowAttachments[(*i)->attachIndex] = false;

			ISceneNode* n = (ISceneNode*)entry->node;
			removeChild(n);
			delete entry;
			i = AttachmentEntries.erase(i);

			if (slot == CS_HEAD)
			{
				HelmNode = NULL;
				Character->HelmHideHair = false;
				HideHelmHair = false;
			}
		}
		else
			++i;
	}
}

void CM2SceneNode::removeAllM2ModelEquipments()
{
	for (T_AttachmentEntries::iterator i=AttachmentEntries.begin(); i != AttachmentEntries.end(); ++i)
	{
		ISceneNode* node = (ISceneNode*)(*i)->node;
		removeChild(node);
		delete (*i);
	}
	AttachmentEntries.clear();
}

void CM2SceneNode::showHelm( bool show )
{
	if (HelmNode)
	{
		HelmNode->Visible = show;
		if (!show)
			Character->HelmHideHair = false;
		else 
			Character->HelmHideHair = HideHelmHair;
	}
}

void CM2SceneNode::showCape(bool show)
{
	Character->ShowCape = show;
}

bool CM2SceneNode::isShowHelm() const
{
	if (Mesh->getType() != MT_CHARACTER)
		return false;

	if (HelmNode)
		return HelmNode->Visible;
	else
		return false;
}

bool CM2SceneNode::isShowCape() const
{
	if (Mesh->getType() != MT_CHARACTER)
		return false;

	return Character->ShowCape;
}

void CM2SceneNode::setParticleSpeed(float speed)
{
	f32 particleSpeed = Animation.getAnimationSpeed() * speed;
	for (T_ParticleSystemNodes::iterator i=ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		(*i)->setSpeed(particleSpeed);
	}
}

void CM2SceneNode::updateAttachmentEntry( SAttachmentEntry* entry )
{
	const SModelAttachment& attachment = Mesh->Attachments[entry->attachIndex];
	s32 bIdx = attachment.boneIndex;
	if(bIdx == -1)
		return;

	const SDynBone& b = Character->DynBones[bIdx];

	IM2SceneNode* node = (IM2SceneNode*)entry->node;

	matrix4 m;
	if (entry->scale != 1.0f)
		m.setScale(entry->scale);
	m.setTranslation(attachment.position);
	m = b.mat * m;

	node->setRelativeTransformation(m);
}

void CM2SceneNode::getBillboardBoneMat(matrix4& m)
{
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	if (!cam)
		return;

 	matrix4 invView = cam->getInverseViewMatrix();	
 	m[0] = invView[0]; m[4] = invView[4]; m[8] = invView[8]; 
  	m[1] = invView[1]; m[5] = invView[5]; m[9] = invView[9];
  	m[2] = invView[2]; m[6] = invView[6]; m[10] = invView[10];

	matrix4 mat;

	vector3df worldrot;
	if (Parent)
	{
		worldrot = Parent->getAbsoluteTransformation().getRotationRadians();
 		worldrot = vector3df(0, PI/2, 0) - worldrot;
	}
	else
	{
		worldrot = getAbsoluteTransformation().getRotationRadians();
		worldrot = vector3df(0, PI/2, 0) - worldrot;
	}

  	mat.setRotationRadians(worldrot);				//wow的billboard需要的附加旋转
   	m = m * mat;
}

IParticleSystemSceneNode* CM2SceneNode::addParticleSystemSceneNode( ParticleSystem* ps )
{
	if (!ps)
		return NULL;

	return new CParticleSystemSceneNode(ps, this);
}

IRibbonSceneNode* CM2SceneNode::addRibbonEmitterSceneNode( RibbonEmitter* re )
{
	if (!re)
		return NULL;

	return new CRibbonSceneNode(re, this);
}

void CM2SceneNode::updateCharacter()
{
	if (Mesh->getType() == MT_CHARACTER)
	{
		if (!IsNpc)
			Character->updateCharacter();
	}
}

void CM2SceneNode::buildVisibleGeosets()
{
	Character->buildVisibleGeosets();
}

bool CM2SceneNode::updateNpc( s32 npcid )
{
	if (Mesh->getType() == MT_CHARACTER && IsNpc)
	{
		bool success = Character->updateNpc(npcid);
		if (success)
		{
			for (u32 slot=0; slot<NUM_CHAR_SLOTS; ++slot)
			{
				s32 id = Character->Equipments[slot];
				if (Character->slotHasModel(slot))
					setM2ModelEquipment(slot, id);
			}
			Character->updateCharacter();
		}
		return success;
	}
	else if(Mesh->getType() == MT_CREATRUE)
	{
		return Character->updateNpc(npcid);
	}

	return false;
}

bool CM2SceneNode::setModelCamera( s32 index )
{
	if (index < 0 || index >= (s32)Mesh->NumModelCameras)
	{
		CurrentCamera = -1;

		for(T_AttachmentEntries::iterator i=AttachmentEntries.begin(); i != AttachmentEntries.end(); ++i)
		{
			CM2SceneNode* node = (CM2SceneNode*)(*i)->node;
			node->CurrentCamera = -1;
		}

		for (T_ParticleSystemNodes::iterator i = ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
		{
			IParticleSystemSceneNode* node = (IParticleSystemSceneNode*)(*i);
			node->setOwnView(NULL);
			node->setOwnProjection(NULL);
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
	for(T_AttachmentEntries::iterator i=AttachmentEntries.begin(); i != AttachmentEntries.end(); ++i)
	{
		CM2SceneNode* node = (CM2SceneNode*)(*i)->node;
		node->CurrentCamera = index;
		node->CurrentProjection = CurrentProjection;
		node->CurrentView = CurrentView;
	}

	//particles
	for (T_ParticleSystemNodes::iterator i = ParticleSystemNodes.begin(); i != ParticleSystemNodes.end(); ++i)
	{
		IParticleSystemSceneNode* node = (IParticleSystemSceneNode*)(*i);
		node->setOwnView(&CurrentView);
		node->setOwnProjection(&CurrentProjection);
	}

	return true;
}

void CM2SceneNode::updateCloseHands()
{
	Character->CloseLHand = Character->Equipments[CS_HAND_LEFT] != 0;
	Character->CloseRHand = Character->Equipments[CS_HAND_RIGHT] != 0;	
}
