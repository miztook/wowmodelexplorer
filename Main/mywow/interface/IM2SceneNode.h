#pragma once

#include "ISceneNode.h"
#include "SColor.h"

class IFileM2;
class wow_m2instance;
class wow_m2appearance;
class wow_m2FSM;
class wow_m2Move;
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

	virtual uint32_t onFillVertexBuffer(uint32_t geoset, SVertex_PCT* vertices, uint32_t vcount) const = 0;

	virtual bool isNpc() const = 0;
	virtual void buildVisibleGeosets() = 0;
	virtual void updateCharacter() = 0;
	virtual bool updateNpc(int32_t npcid) = 0;

	virtual const aabbox3df& getAnimatedWorldAABB() const = 0;
	virtual const aabbox3df& getWorldBoundingAABox() const = 0;
	virtual const aabbox3df& getWorldCollisionAABox() const = 0;
	virtual float getWorldRadius() const = 0;

	virtual bool playAnimationByIndex(uint32_t anim, bool loop, int32_t timeblend = 200) = 0;
	virtual bool playAnimationByName(const char* name, uint32_t subIdx, bool loop, int32_t timeblend = 200) = 0;
	virtual int32_t getCurrentAnimationIndex() const = 0;	
	virtual void setParticleSpeed(float speed) = 0;
	virtual float getParticleSpeed() const = 0;
	virtual int32_t getCurrentAnimation(animation* anim = nullptr) const = 0;

	virtual bool setModelCamera(int32_t index) = 0;
	virtual int32_t getModelCamera() const = 0;

	virtual void setM2ModelEquipment(int32_t slot, int32_t itemid, bool sheath) = 0;
	virtual bool setMountM2SceneNode(IM2SceneNode* m2Node) = 0;

	virtual void setModelAlpha(bool enable, float val) = 0;
	virtual void setModelColor(bool enable, SColor color) = 0;
	virtual void enableFog(bool enable) = 0;

	virtual IParticleSystemSceneNode* addParticleSystemSceneNode(ParticleSystem* ps) = 0;
	virtual IRibbonSceneNode* addRibbonEmitterSceneNode(RibbonEmitter* re) = 0;

public:
	animation		Animation;
	E_RENDERINST_TYPE		RenderInstType;
	int32_t	RenderPriority;
};