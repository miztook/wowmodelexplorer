#pragma once

#include "ISceneNode.h"
#include "SColor.h"

class IFileM2;
class wow_m2instance;
class wow_m2appearance;
class wow_m2FSM;
class wow_m2Move;
class wow_m2spell;
class ITexture;
class IParticleSystemSceneNode;
class IRibbonSceneNode;
class ParticleSystem;
class RibbonEmitter;
struct SVertex_PCT;
struct SSpellVKInfo;

class IM2SceneNode : public ISceneNode
{
public:
	explicit IM2SceneNode(ISceneNode* parent)
		: ISceneNode(parent), RenderPriority(0), RenderInstType(ERT_MESH) { Type = EST_M2; }

	virtual ~IM2SceneNode() {}

public:
	virtual const IFileM2* getFileM2() const = 0;
	virtual wow_m2instance* getM2Instance() const = 0;
	virtual wow_m2appearance* getM2Appearance() const = 0;
	virtual wow_m2FSM* getM2FSM() const = 0;
	virtual wow_m2Move* getM2Move() const = 0;
	virtual wow_m2spell* getM2Spell() const = 0;

	virtual u32 onFillVertexBuffer(u32 geoset, SVertex_PCT* vertices, u32 vcount) const = 0;

	virtual bool isNpc() const = 0;
	virtual void buildVisibleGeosets() = 0;
	virtual void updateCharacter() = 0;
	virtual bool updateNpc(s32 npcid) = 0;

	virtual const aabbox3df& getAnimatedWorldAABB() const = 0;
	virtual const aabbox3df& getWorldBoundingAABox() const = 0;
	virtual const aabbox3df& getWorldCollisionAABox() const = 0;
	virtual f32 getWorldRadius() const = 0;

	virtual bool playAnimationByIndex(u32 anim, bool loop, s32 timeblend = 200) = 0;
	virtual bool playAnimationByName(const c8* name, u32 subIdx, bool loop, s32 timeblend = 200) = 0;
	virtual s32 getCurrentAnimationIndex() const = 0;	
	virtual void setParticleSpeed(f32 speed) = 0;
	virtual f32 getParticleSpeed() const = 0;
	virtual s32 getCurrentAnimation(animation* anim = NULL_PTR) const = 0;

	virtual bool setModelCamera(s32 index) = 0;
	virtual s32 getModelCamera() const = 0;

	virtual void setM2ModelEquipment(s32 slot, s32 itemid, bool sheath) = 0;
	virtual bool setMountM2SceneNode(IM2SceneNode* m2Node) = 0;
	virtual void setSpellVisualKit(u32 spellvkId) = 0;
	virtual void removeSpellVisualKit( IM2SceneNode* node ) = 0;
	virtual void removeAllSpellVisualKits() = 0;

	virtual void setModelAlpha(bool enable, f32 val) = 0;
	virtual void setModelColor(bool enable, SColor color) = 0;
	virtual void enableFog(bool enable) = 0;

	virtual IParticleSystemSceneNode* addParticleSystemSceneNode(ParticleSystem* ps) = 0;
	virtual IRibbonSceneNode* addRibbonEmitterSceneNode(RibbonEmitter* re) = 0;

public:
	animation		Animation;
	E_RENDERINST_TYPE		RenderInstType;
	s32	RenderPriority;
};