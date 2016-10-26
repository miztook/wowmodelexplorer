#pragma once

#include "IM2SceneNode.h"
#include "SMaterial.h"
#include "wow_def.h"
#include <vector>
#include <list>

class IParticleSystemSceneNode;
class IRibbonSceneNode;
class ISceneManager;
class CFileM2;

class CM2SceneNode : public IM2SceneNode
{
public:
	CM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc);
	virtual ~CM2SceneNode();

public:
	//IM2SceneNode
	virtual const IFileM2* getFileM2() const { return (IFileM2*)Mesh; }
	virtual wow_m2instance* getM2Instance() const { return M2Instance; }
	virtual wow_m2appearance* getM2Appearance() const { return M2Appearance; }
	virtual wow_m2FSM* getM2FSM() const { return M2FSM; }
	virtual wow_m2Move* getM2Move() const { return M2Move; }
	virtual wow_m2spell* getM2Spell() const { return M2Spell; }

	virtual u32 onFillVertexBuffer(u32 geoset, SVertex_PCT* vertices, u32 vcount) const;

	virtual bool isNpc() const { return IsNpc; }
	virtual void updateCharacter();
	virtual void buildVisibleGeosets();
	virtual bool updateNpc(s32 npcid);	

	virtual const aabbox3df& getAnimatedWorldAABB() const { return AnimatedWorldAABB; }
	virtual const aabbox3df& getWorldBoundingAABox() const { return WorldBoundingAABox; }
	virtual const aabbox3df& getWorldCollisionAABox() const { return WorldCollisionAABox; }
	virtual f32 getWorldRadius() const { return WorldRadius; }
	virtual bool playAnimationByIndex(u32 anim, bool loop, s32 timeblend = 200);
	virtual bool playAnimationByName(const c8* name, u32 subIdx, bool loop,  s32 timeblend = 200);
	virtual s32 getCurrentAnimationIndex() const { return CurrentAnim; }
	virtual void setParticleSpeed(f32 speed);
	virtual f32 getParticleSpeed() const { return ParticleSpeed; }
	virtual s32 getCurrentAnimation(animation* anim = NULL_PTR) const {  if(anim) *anim = Animation; return CurrentAnim; }

	virtual bool setModelCamera(s32 index);
	virtual s32 getModelCamera() const { return CurrentCamera; }
	virtual void setM2ModelEquipment(s32 slot, s32 itemid, bool sheath);
	virtual bool setMountM2SceneNode(IM2SceneNode* m2Node);
	virtual void setSpellVisualKit(u32 spellvkId);
	virtual void removeSpellVisualKit( IM2SceneNode* node );
	virtual void removeAllSpellVisualKits();

	virtual IParticleSystemSceneNode* addParticleSystemSceneNode(ParticleSystem* ps);
	virtual IRibbonSceneNode* addRibbonEmitterSceneNode(RibbonEmitter* re);

	virtual void setModelAlpha(bool enable, f32 val);
	virtual void setModelColor(bool enable, SColor color);
	virtual void enableFog(bool enable) { EnableFog = enable; }

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const;
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const;
	virtual void onUpdated();

protected:
	void updateAttachmentEntry(const SAttachmentEntry* entry);
	void updateSpellEffectEntry(const SAttachmentEntry* entry);

	void renderGeoset(u32 index) const;

	void removeM2ModelEquipment( s32 slot );
	void removeAllM2Attachments();
	void updateLeftCloseHand(bool sheath);
	void updateRightCloseHand(bool sheath);

	bool checkFrameLimit(u32& timeSinceLastFrame, s32 limit);

	void tickInvisible(u32 timeSinceStart, u32 timeSinceLastFrame);
	void tickVisible(u32 timeSinceStart, u32 timeSinceLastFrame);

	bool removeMountM2SceneNode();

protected:
	struct SFadeEntry
	{
		u32 time;
		u32 current;
		f32	alpha;
		IM2SceneNode* node;
	};

protected:
	CFileM2* Mesh;
	wow_m2instance*		M2Instance;
	wow_m2appearance*		M2Appearance;
	wow_m2FSM*	M2FSM;
	wow_m2Move* M2Move;
	wow_m2spell* M2Spell;

	aabbox3df		AnimatedWorldAABB;
	aabbox3df	WorldBoundingAABox;
	aabbox3df	WorldCollisionAABox;

	f32		ParticleSpeed;
	f32		WorldRadius;
	f32		Scale;	
	matrix4		CurrentView;
	matrix4		CurrentProjection;

	u32		FrameInterval;

	s32		CurrentAnim;
	s32		TimeBlend;
	s32		AnimTimeBlend;
	s32		CurrentCamera;

	typedef std::list<SAttachmentEntry, qzone_allocator<SAttachmentEntry> >	T_AttachmentList;
	T_AttachmentList		AttachmentList;

	T_AttachmentList		SpellEffectList;

	typedef std::list<SFadeEntry, qzone_allocator<SFadeEntry> > T_SceneNodeList;
	T_SceneNodeList		FadeList;

	typedef std::vector<IParticleSystemSceneNode*>	T_ParticleSystemNodes;
	T_ParticleSystemNodes		ParticleSystemNodes;

	typedef std::vector<IRibbonSceneNode*>	T_RibbonEmitterNodes;
	T_RibbonEmitterNodes		RibbonEmitterNodes;

	bool	IsNpc;
	bool AnimateColors;
	bool Death;
	bool ModelAlpha;
	bool EnableFog;

	//friend
	friend class wow_m2appearance;
	friend class wow_m2spell;
};