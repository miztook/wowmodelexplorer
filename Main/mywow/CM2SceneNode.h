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

	virtual uint32_t onFillVertexBuffer(uint32_t geoset, SVertex_PCT* vertices, uint32_t vcount) const;

	virtual bool isNpc() const { return IsNpc; }
	virtual void updateCharacter();
	virtual void buildVisibleGeosets();
	virtual bool updateNpc(int32_t npcid);	

	virtual const aabbox3df& getAnimatedWorldAABB() const { return AnimatedWorldAABB; }
	virtual const aabbox3df& getWorldBoundingAABox() const { return WorldBoundingAABox; }
	virtual const aabbox3df& getWorldCollisionAABox() const { return WorldCollisionAABox; }
	virtual float getWorldRadius() const { return WorldRadius; }
	virtual bool playAnimationByIndex(uint32_t anim, bool loop, int32_t timeblend = 200);
	virtual bool playAnimationByName(const char* name, uint32_t subIdx, bool loop,  int32_t timeblend = 200);
	virtual int32_t getCurrentAnimationIndex() const { return CurrentAnim; }
	virtual void setParticleSpeed(float speed);
	virtual float getParticleSpeed() const { return ParticleSpeed; }
	virtual int32_t getCurrentAnimation(animation* anim = nullptr) const {  if(anim) *anim = Animation; return CurrentAnim; }

	virtual bool setModelCamera(int32_t index);
	virtual int32_t getModelCamera() const { return CurrentCamera; }
	virtual void setM2ModelEquipment(int32_t slot, int32_t itemid, bool sheath);
	virtual bool setMountM2SceneNode(IM2SceneNode* m2Node);

	virtual IParticleSystemSceneNode* addParticleSystemSceneNode(ParticleSystem* ps);
	virtual IRibbonSceneNode* addRibbonEmitterSceneNode(RibbonEmitter* re);

	virtual void setModelAlpha(bool enable, float val);
	virtual void setModelColor(bool enable, SColor color);
	virtual void enableFog(bool enable) { EnableFog = enable; }

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const;
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const;
	virtual void onUpdated();

protected:
	void updateAttachmentEntry(const SAttachmentEntry* entry);

	void renderGeoset(uint32_t index) const;

	void removeM2ModelEquipment( int32_t slot );
	void removeAllM2Attachments();
	void updateLeftCloseHand(bool sheath);
	void updateRightCloseHand(bool sheath);

	bool checkFrameLimit(uint32_t& timeSinceLastFrame, int32_t limit);

	void tickInvisible(uint32_t timeSinceStart, uint32_t timeSinceLastFrame);
	void tickVisible(uint32_t timeSinceStart, uint32_t timeSinceLastFrame);

	bool removeMountM2SceneNode();

protected:
	struct SFadeEntry
	{
		uint32_t time;
		uint32_t current;
		float	alpha;
		IM2SceneNode* node;
	};

protected:
	CFileM2* Mesh;
	wow_m2instance*		M2Instance;
	wow_m2appearance*		M2Appearance;
	wow_m2FSM*	M2FSM;
	wow_m2Move* M2Move;

	aabbox3df		AnimatedWorldAABB;
	aabbox3df	WorldBoundingAABox;
	aabbox3df	WorldCollisionAABox;

	float		ParticleSpeed;
	float		WorldRadius;
	float		Scale;	
	matrix4		CurrentView;
	matrix4		CurrentProjection;

	uint32_t		FrameInterval;

	int32_t		CurrentAnim;
	int32_t		TimeBlend;
	int32_t		AnimTimeBlend;
	int32_t		CurrentCamera;

	std::list<SAttachmentEntry, qzone_allocator<SAttachmentEntry> >		AttachmentList;
	std::list<SFadeEntry, qzone_allocator<SFadeEntry> >		FadeList;
	std::vector<IParticleSystemSceneNode*>		ParticleSystemNodes;
	std::vector<IRibbonSceneNode*>		RibbonEmitterNodes;

	bool	IsNpc;
	bool AnimateColors;
	bool Death;
	bool ModelAlpha;
	bool EnableFog;

	//friend
	friend class wow_m2appearance;
};