#pragma once

#include "CM2SceneNode.h"
#include "editor_structs.h"

class CEditM2SceneNode : public CM2SceneNode
{
public:
	CEditM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc);
	virtual ~CEditM2SceneNode();

public:
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible) override;
	virtual void render() const override;

public:
	bool	isPlaying() const { return !Paused; }
	void	pause() { Paused = true; }
	void	resume() { Paused = false; }
	void	stop() { Animation.setCurrentFrame(0); pause(); }
	void	step(f32 frame) { 
		Animation.setCurrentFrame(Animation.getCurrentFrame() + frame); 
		pause(); 
		TickDelta = (u32)(frame * Animation.getAnimationSpeed()); }

public:
	void showShoulder(bool show);
	void showLeftHand(bool show);
	void showRightHand(bool show);
	void showParticles(bool show);
	void showGeoset(u32 index, bool show);
	bool isGeosetShow(u32 index);
	void showAttachment(u32 index, bool show);
	bool isAttachmentShow(u32 index);
	void setOverrideWireframe(editor::E_OVERRIDE_WIREFRAME wireframe);

	bool getChildSceneNodes(editor::SM2ChildSceneNodes* childSceneNodes);
	const c8* getReplaceTextureFileName(editor::E_MODEL_REPLACE_TEXTURE texture);
	const c8* getTextureFileName(u32 index);
	const c8* getRegionTextureFileName(ECharRegions region);

	void getGeosetDebugInfo(c16* msg, u32 size);
	void drawBoundingBox(SColor color);
	void drawBoundingAABox(SColor color);
	void drawCollisionAABox(SColor color);
	void drawBones(SColor color);
	
private:
	void renderGeoset(u32 index) const;

	void drawBone(u32 idx, SColor color);
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
	u32		TickDelta;
	bool*	ShowGeosets;
	bool*	IsGeosetsVisible;
	bool*	BonesDrawn;

	SAttachment*		Attachments;

	friend class CEditSceneManager;
};