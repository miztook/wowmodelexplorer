#include "stdafx.h"
#include "wow_m2States.h"
#include "mywow.h"
#include "mywowclient.h"

bool wow_m2State_Stand::enter()
{
	const c8* actname = getActionName(EMS_STAND);
	M2Action = Entity->getFileM2()->getAction(actname);
	if(!M2Action || !Entity->getM2FSM()->playAction(M2Action, 200))
	{
		exit();
		return false;
	}

	return true;
}

bool wow_m2State_Stand::isValid() const
{
	const c8* actname = getActionName(EMS_STAND);
	return Entity->getFileM2()->getAction(actname) != NULL;
}

bool wow_m2State_commonAction::enter()
{
	const c8* actname = getActionName(State);
	M2Action = Entity->getFileM2()->getAction(actname);
	if(!M2Action || !Entity->getM2FSM()->playAction(M2Action, 200))
	{
		exit();
		return false;
	}

	return true;
}

f32 wow_m2State_Jump::GRAVITY = 0.001f * 0.05f;
f32 wow_m2State_Jump::ABOUTLANDHEIGHT = 2.0f;
f32 wow_m2State_Jump::LANDHEIGHT = 0.4f;

bool wow_m2State_Jump::isValid() const
{
	const c8* actname = getActionName(EMS_JUMP);
	return Entity->getFileM2()->getAction(actname) != NULL;
}

bool wow_m2State_Jump::enter()
{
	const c8* actname = getActionName(EMS_JUMP);
	M2Action = Entity->getFileM2()->getAction(actname);
	if(!M2Action || !Entity->getM2FSM()->playAction(M2Action, 200))
	{
		exit();
		return false;
	}

	//起始高度
	JumpDir.clear();
	HeightGround = Entity->getM2Move()->getPos().Y;
	CurrentVelocity = Velocity;
	IsAboutLand =false;
	NextLandRun = false;
	WaitToFinish = false;

	return true;
}

void wow_m2State_Jump::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	wow_m2FSM* fsm = Entity->getM2FSM();
	f32 height = getM2Height();
	if (fsm->isPlaying(M2Action))
	{
		f32 v = CurrentVelocity;
		CurrentVelocity -= GRAVITY * timeSinceLastFrame;
		f32 delta = (v + CurrentVelocity) * 0.5f * timeSinceLastFrame;

		if (HeightGround + delta > height)				//在地面之上
		{
			HeightGround += delta;
		}
		else				//落地
		{
			HeightGround = height;
			JumpDir.clear();

			if (NextLandRun || WaitToFinish)
			{
				Entity->getM2Move()->setDestHeight(0.0f);
				Entity->getM2FSM()->changeState(NextLandRun ? EMS_RUN : EMS_STAND);
				return;
			}
		}
		Entity->getM2Move()->setDestHeight(HeightGround - height);

		const SDynAction& dynAction = fsm->getDynAction();
		if (dynAction.currentIndex == 0 && CurrentVelocity <= 0)			//到达最高点
		{
			IsAboutLand = false;
			NextLandRun = false;
			WaitToFinish = false;

			if(!fsm->advanceAnimation() || !Entity->playAnimationByIndex(fsm->getCurrentAnimationIndex(), false))
				WaitToFinish = true;
		}
		else if (dynAction.currentIndex == 1)
		{
			if (HeightGround< height + LANDHEIGHT)			//跳跃结束到下一个动作
			{
				if(!fsm->advanceAnimation())
					WaitToFinish = true;
				else
				{
					if (NextLandRun)			//落地行走
					{
						if (!Entity->playAnimationByName("JumpLandRun", 0, false))
						{
							if(!Entity->playAnimationByIndex(fsm->getCurrentAnimationIndex(), false))
								WaitToFinish = true;
						}
					}
					else
					{
						if(!Entity->playAnimationByIndex(fsm->getCurrentAnimationIndex(), false))
							WaitToFinish = true;
					}
				}
			}
			else if (CurrentVelocity < 0.0f && HeightGround < ABOUTLANDHEIGHT + height)
			{
				IsAboutLand = true;
			}
		}
	}
	else
	{
		Entity->getM2Move()->setDestHeight(0.0f);
		Entity->getM2FSM()->changeState(NextLandRun ? EMS_RUN : EMS_STAND);
	}	
}

bool wow_m2State_Jump::isJumpEnd() const
{
	//在播放 jump end
	wow_m2FSM* fsm = Entity->getM2FSM();
	if (M2Action && fsm->isPlaying(M2Action))
	{
		const SDynAction& dynAction = fsm->getDynAction();
		return dynAction.currentIndex == 2 &&
			Entity->getCurrentAnimation() == fsm->getCurrentAnimationIndex();			//jump end playing
	}

	return false;
}

void wow_m2State_Jump::onAnimationEnd( u32 currentIndex )
{
	wow_m2FSM* fsm = Entity->getM2FSM();
	if (M2Action && fsm->isPlaying(M2Action))
	{
		f32 height = getM2Height();
		const SDynAction& dynAction = fsm->getDynAction();
		if(dynAction.currentIndex == 0)
		{
			//开始跳跃
			IsAboutLand = false;
			NextLandRun = false;
			WaitToFinish = false;

			if(!fsm->advanceAnimation() || !Entity->playAnimationByIndex(fsm->getCurrentAnimationIndex(), false))
				WaitToFinish = true;
		}
		else if (dynAction.currentIndex == 1)
		{
			if (HeightGround > height + LANDHEIGHT)			//继续跳跃
			{
				if (!Entity->playAnimationByIndex(fsm->getCurrentAnimationIndex(), false))
					WaitToFinish = true;
			
				if (CurrentVelocity < 0.0f && HeightGround < ABOUTLANDHEIGHT + height)
					IsAboutLand = true;
			}
		}
		else if (dynAction.currentIndex == 2)
		{
			Entity->getM2Move()->setDestHeight(0.0f);
			Entity->getM2FSM()->changeState(NextLandRun ? EMS_RUN : EMS_STAND);
		}
	}
}

f32 wow_m2State_Jump::getM2Height() const
{
	vector3df pos = Entity->getM2Move()->getPos();

	return g_Client->getWorld()->getHeightNormal(pos.X, pos.Z);
}

void wow_m2State_commonAction::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
 	if (M2Action && !Entity->getM2FSM()->isPlaying(M2Action))
		Entity->getM2FSM()->changeState(EMS_STAND);
}

void wow_m2State_commonAction::exit()
{
	M2Action = NULL;
}

bool wow_m2State_commonAction::isValid( ) const
{
	const c8* actname = getActionName(State);
	return Entity->getFileM2()->getAction(actname) != NULL;
}
