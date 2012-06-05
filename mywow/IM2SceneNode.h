#pragma once

#include "ISceneNode.h"

class IFileM2;
class wow_character;
class ITexture;
class IParticleSystemSceneNode;
class IRibbonSceneNode;
class ParticleSystem;
class RibbonEmitter;

class IM2SceneNode : public ISceneNode
{
public:
	IM2SceneNode(ISceneNode* parent, E_RENDERINST_TYPE renderType = ERT_MESH)
		: ISceneNode(parent, renderType) {}

	virtual ~IM2SceneNode() {}

public:
	virtual IFileM2* getFileM2() const = 0;
	virtual wow_character* getWowCharacter() const  = 0;
	virtual void buildVisibleGeosets() = 0;
	virtual void updateCharacter() = 0;
	virtual bool updateNpc(s32 npcid) = 0;

	virtual bool playAnimationByIndex(u32 anim, bool loop, f32 speed = 1.0f, s32 timeblend = 200, bool restart = true) = 0;
	virtual bool playAnimationByName(const c8* name, u32 subIdx, bool loop, f32 speed = 1.0f, s32 timeblend = 200, bool restart = true) = 0;
	virtual bool takeItem(s32 itemid, s32* itemslot) = 0;
	virtual void loadStartOutfit(s32 startid, bool deathknight) = 0;
	virtual void loadSet(s32 setid) = 0;
	virtual bool isNpc() const = 0;
	virtual s32 getCurrentAnimationIndex() const = 0;
	virtual bool setModelCamera(s32 index) = 0;
	virtual s32 getModelCamera() const = 0;
	virtual void setOwnProjection(matrix4* projection) = 0;
	virtual void setM2ModelEquipment(s32 slot, s32 itemid) = 0;

	virtual void showHelm(bool show) = 0;
	virtual void showCape(bool show) = 0;
	virtual void setParticleSpeed(float speed) = 0;

	virtual bool isShowHelm() const = 0;
	virtual bool isShowCape() const = 0;

	virtual IParticleSystemSceneNode* addParticleSystemSceneNode(ParticleSystem* ps) = 0;
	virtual IRibbonSceneNode* addRibbonEmitterSceneNode(RibbonEmitter* re) = 0;

public:
	animation		Animation;
};