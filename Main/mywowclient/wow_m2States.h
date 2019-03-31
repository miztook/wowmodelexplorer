#pragma once

#include "base.h"
#include "wow_m2State.h"
#include "wow_m2action.h"

#include "wow_m2Actions.h"

//m2µÄ×´Ì¬

class IM2SceneNode;

//ÐÝÏÐ
class wow_m2State_Stand : public wow_m2State<IM2SceneNode>
{
public:
	explicit wow_m2State_Stand(IM2SceneNode* m2sceneNode) : wow_m2State(m2sceneNode), M2Action(NULL) {}

	virtual bool isValid() const;
	virtual bool enter();
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame) {}
	virtual void exit() { M2Action= NULL; }
	virtual E_M2_STATES getType() const { return EMS_STAND; }

	virtual wow_m2Action* getM2Action() const { return M2Action; }
	virtual void onAnimationEnd(uint32_t currentIndex) {}

private:
	wow_m2Action* M2Action;
};

//ÌøÔ¾
class wow_m2State_Jump : public wow_m2State<IM2SceneNode>
{
public:
	explicit wow_m2State_Jump(IM2SceneNode* m2sceneNode)
		: wow_m2State(m2sceneNode), M2Action(NULL),
		IsAboutLand(false), NextLandRun(false), WaitToFinish(false)
	{
		JumpDir.clear();
		Velocity = 0.02f;
		CurrentVelocity = Velocity;

		HeightGround = 0.0f;
	}

	static float GRAVITY;
	static float LANDHEIGHT;
	static float ABOUTLANDHEIGHT;

	virtual bool isValid() const;
	virtual bool enter();
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame);
	virtual void exit() { M2Action= NULL; }
	virtual E_M2_STATES getType() const { return EMS_JUMP; }

	virtual wow_m2Action* getM2Action() const { return M2Action; }
	virtual void onAnimationEnd(uint32_t currentIndex);

	bool isJumpEnd() const;
	bool isAboutLand() const { return IsAboutLand; }
	void setNextLandRun(bool nextLand) { NextLandRun = nextLand; }
	void setJumpDir(const vector3df& dir) { JumpDir = dir; }
	const vector3df& getJumpDir() const { return JumpDir; }

private:
	float getM2Height() const;

private:
	wow_m2Action*	M2Action;
	vector3df	JumpDir;
	float	Velocity;
	float	CurrentVelocity;
	float	HeightGround;
	bool IsAboutLand;
	bool NextLandRun;
	bool WaitToFinish;
};

//Ë³Ðò²¥·Å¶¯×÷
class wow_m2State_commonAction : public wow_m2State<IM2SceneNode>
{
public:
	wow_m2State_commonAction(IM2SceneNode* m2SceneNode, E_M2_STATES state)
		: wow_m2State(m2SceneNode), M2Action(NULL), State(state) {}

	virtual bool isValid() const;
	virtual bool enter();
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame);
	virtual void exit();
	virtual E_M2_STATES getType() const { return State; }

	virtual wow_m2Action* getM2Action() const { return M2Action; }
	virtual void onAnimationEnd(uint32_t currentIndex) {}

private:
	wow_m2Action* M2Action;
	E_M2_STATES	State;
};
