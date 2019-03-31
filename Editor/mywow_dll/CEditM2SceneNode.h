#pragma once

#include "CM2SceneNode.h"
#include "editor_structs.h"

class CEditM2SceneNode : public CM2SceneNode
{
public:
	CEditM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc);
	virtual ~CEditM2SceneNode();

public:
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible) override;
	virtual void render() const override;

public:
	bool	isPlaying() const { return !Paused; }
	void	pause() { Paused = true; }
	void	resume() { Paused = false; }
	void	stop() { Animation.setCurrentFrame(0); pause(); }
	void	step(float frame) { 
		Animation.setCurrentFrame(Animation.getCurrentFrame() + frame); 
		pause(); 
		TickDelta = (uint32_t)(frame * Animation.getAnimationSpeed()); }

public:
	void showShoulder(bool show);
	void showLeftHand(bool show);
	void showRightHand(bool show);
	void showParticles(bool show);
	void showGeoset(uint32_t index, bool show);
	bool isGeosetShow(uint32_t index);
	void showAttachment(uint32_t index, bool show);
	bool isAttachmentShow(uint32_t index);
	void setOverrideWireframe(editor::E_OVERRIDE_WIREFRAME wireframe);

	bool getChildSceneNodes(editor::SM2ChildSceneNodes* childSceneNodes);
	const char* getReplaceTextureFileName(editor::E_MODEL_REPLACE_TEXTURE texture);
	const char* getTextureFileName(uint32_t index);
	const char* getRegionTextureFileName(ECharRegions region);

	void getGeosetDebugInfo(char16_t* msg, uint32_t size);
	void drawBoundingBox(SColor color);
	void drawBoundingAABox(SColor color);
	void drawCollisionAABox(SColor color);
	void drawBones(SColor color);
	
private:
	void renderGeoset(uint32_t index) const;

	void drawBone(uint32_t idx, SColor color);
	void updateAttachmentSceneNodes();

private:
	struct  SAttachment
	{
		IMeshSceneNode* attachmentSceneNode;
		ICoordSceneNode* coordSceneNode;
		bool show;
	};

	editor::E_OVERRIDE_WIREFRAME		OverrideWireframe;

public:
	bool	ShowBones;
	bool	ShowBoundingBox;
	bool	ShowModel;
	bool ShowBoundingAABox;
	bool ShowCollisionAABox;

private:
	bool	Paused;
	uint32_t		TickDelta;
	bool*	ShowGeosets;
	bool*	IsGeosetsVisible;
	bool*	BonesDrawn;

	SAttachment*		Attachments;

	friend class CEditSceneManager;
};