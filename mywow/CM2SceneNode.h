#pragma once

#include "IM2SceneNode.h"
#include "SMaterial.h"
#include "IParticleSystemSceneNode.h"
#include "IRibbonSceneNode.h"
#include <list>

class ISceneManager;
struct SAttachmentEntry;

class CM2SceneNode : public IM2SceneNode
{
public:
	CM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc);
	virtual ~CM2SceneNode();

public:
	//IM2SceneNode
	virtual IFileM2* getFileM2() const { return Mesh; }
	virtual wow_character* getWowCharacter() const { return Character; }
	virtual void updateCharacter();
	virtual void buildVisibleGeosets();
	virtual bool updateNpc(s32 npcid);	

	virtual bool playAnimationByIndex(u32 anim, bool loop, f32 speed = 1.0f, s32 timeblend = 200, bool restart = true);
	virtual bool playAnimationByName(const c8* name, u32 subIdx, bool loop,  f32 speed = 1.0f, s32 timeblend = 200, bool restart = true);
	virtual bool takeItem(s32 itemid, s32* itemslot);
	virtual void loadStartOutfit(s32 startid, bool deathknight);
	virtual void loadSet(s32 setid);
	virtual bool isNpc() const { return IsNpc; }
	virtual s32 getCurrentAnimationIndex() const { return CurrentAnim; }
	virtual bool setModelCamera(s32 index);
	virtual s32 getModelCamera() const { return CurrentCamera; }
	virtual void setOwnProjection(matrix4* projection) { OwnProjection = projection; }
	virtual void setM2ModelEquipment(s32 slot, s32 itemid);

	virtual void showHelm(bool show);
	virtual void showCape(bool show);
	virtual void setParticleSpeed(float speed);

	virtual bool isShowHelm() const;
	virtual bool isShowCape() const;

	virtual IParticleSystemSceneNode* addParticleSystemSceneNode(ParticleSystem* ps);
	virtual IRibbonSceneNode* addRibbonEmitterSceneNode(RibbonEmitter* re);

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck = true);
	virtual aabbox3df getBoundingBox() const;
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame);
	virtual void render();
	virtual bool isNodeEligible();

protected:
	void updateAttachmentEntry(SAttachmentEntry* entry);

	void getBillboardBoneMat(matrix4& mat);				//给billboard的骨骼的旋转矩阵 

	void renderGeoset(u32 index);

	void removeM2ModelEquipment( s32 slot );
	void removeAllM2ModelEquipments();
	void updateCloseHands();

protected:
	bool	IsNpc;

	IFileM2* Mesh;
	wow_character*		Character;

	s32		CurrentAnim;
	string64		CurrentAnimName;

	s32		TimeBlend;
	s32		AnimTimeBlend;
	bool		Restart;

	s32		CurrentCamera;
	matrix4		CurrentView;
	matrix4		CurrentProjection;

	matrix4*		OwnProjection;

	typedef std::list<SAttachmentEntry*, qzone_allocator<SAttachmentEntry*>>  T_AttachmentEntries;
	T_AttachmentEntries		AttachmentEntries;

	//
	CM2SceneNode*	HelmNode;
	bool HideHelmHair;
	bool AnimateColors;
	bool Death;

	typedef std::list<IParticleSystemSceneNode*, qzone_allocator<IParticleSystemSceneNode*>>	T_ParticleSystemNodes;
	T_ParticleSystemNodes		ParticleSystemNodes;

	typedef std::list<IRibbonSceneNode*, qzone_allocator<IRibbonSceneNode*>>	T_RibbonEmitterNodes;
	T_RibbonEmitterNodes		RibbonEmitterNodes;
};