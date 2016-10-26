#include "StdAfx.h"
#include "CPlayer.h"
#include "mywowclient.h"
#include "wow_m2TargetCamera.h"
#include "wow_m2States.h"

CPlayer::CPlayer()
{
	TargetCamera = new wow_m2TargetCamera(1.0f, 2000.0f, PI / 4.0f);

	WalkSpeed = 0.005f;
	RunSpeed = 0.01f;
	RollSpeed = 0.015f;
	BackwardsSpeed = 0.005f;

	MainModel = 
	RideModel = NULL;

	RideNpcId = 0;
}

CPlayer::~CPlayer()
{
	if(RideModel)
	{
		RideModel->setMountM2SceneNode(NULL);
		g_Engine->getSceneManager()->removeSceneNode(RideModel, true);
		RideModel = NULL;
		RideNpcId = 0;
	}
	delete TargetCamera;
}

void CPlayer::setTargetCamera( f32 nearValue, f32 farValue, f32 fov )
{
	delete TargetCamera;
	TargetCamera = new wow_m2TargetCamera(nearValue, farValue, fov);
}

void CPlayer::setM2AsTarget( IM2SceneNode* node, f32 distance, f32 rad )
{
	TargetCamera->setM2AsTarget(node, distance, rad);
	MainModel = node;
}

void CPlayer::tick( u32 delta )
{
	if (!TargetCamera->getM2Target())
		return;

	SMoveControl moveControl = {0};

    
#ifndef MW_USE_TOUCH_INPUT				//touch control
	IInputReader* inputReader = g_Engine->getInputReader();

	moveControl.front = inputReader->isKeyPressed('W');
	moveControl.back = inputReader->isKeyPressed('S');
	moveControl.left = inputReader->isKeyPressed('A');	
	moveControl.right =inputReader->isKeyPressed('D');	
	moveControl.up = inputReader->isKeyPressed(VK_SPACE);	
	moveControl.down = false;
	moveControl.rightMouse = inputReader->isMousePressed(EMB_RIGHT);
	moveControl.leftMouse = inputReader->isMousePressed(EMB_LEFT);
#else
    
#endif

	if (moveControl.rightMouse && moveControl.leftMouse)
	{
		moveControl.front = true;
	}

	onPlayerSpellAction(delta);

	onPlayerMoveAction(delta, moveControl);

	onPlayerDirection(delta, moveControl);

	TargetCamera->tick(delta);
}

void CPlayer::onMouseWheel( f32 fDelta )
{
	if (!TargetCamera->getM2Target())
		return;
	TargetCamera->onMouseWheel(fDelta);
}

void CPlayer::onMouseMove( f32 pitchDegree, f32 yawDegree )
{
	if (!TargetCamera->getM2Target())
		return;
	TargetCamera->onMouseMove(pitchDegree, yawDegree);
}

void CPlayer::rideOnModel( s32 npcid, E_M2_STATES state )
{
	if (!MainModel)
		return;

	RideNpcId = npcid;

	if(RideModel)
	{
		RideModel->setMountM2SceneNode(NULL);
		MainModel->getM2Move()->setDir(RideModel->getM2Move()->getDir());
		MainModel->getM2Move()->setPos(RideModel->getM2Move()->getPos());
		MainModel->getM2Move()->setUp(vector3df::UnitY());
		g_Engine->getSceneManager()->removeSceneNode(RideModel, true);
		RideModel = NULL;
	}

	if (npcid == 0)
	{
		TargetCamera->setM2AsTarget(MainModel);

		MainModel->getM2FSM()->changeState(state);
	}
	else
	{
		c8 path[256];
		g_Engine->getWowDatabase()->getNpcPath(npcid, false, path, 256);
		IFileM2* m2Ride = g_Engine->getResourceLoader()->loadM2(path);
		RideModel = g_Engine->getSceneManager()->addM2SceneNode(m2Ride, NULL, true);
		if (RideModel)
		{
			RideModel->updateNpc(npcid);

			wow_m2Move* m2Move = RideModel->getM2Move();
			if (m2Move)
			{
				m2Move->setDir(MainModel->getM2Move()->getDir());
				m2Move->setPos(MainModel->getM2Move()->getPos());
			}

			if( !RideModel->setMountM2SceneNode(MainModel))
			{
				RideModel->setMountM2SceneNode(NULL);
				g_Engine->getSceneManager()->removeSceneNode(RideModel, true);
				RideModel = NULL;
				RideNpcId = 0;
				return;
			}
		
			MainModel->getM2FSM()->changeState(state);

			TargetCamera->setM2AsTarget(RideModel);
		}
		else
		{
			RideNpcId = 0;
		}
	}
}

void CPlayer::onPlayerSpellAction( u32 delta )
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	if (!fsm)
		return;

	E_M2_STATES state = fsm->getCurrentState();
	if ((state >= EMS_STAND && state <= EMS_RUN))
	{
		if (CSysUtility::isKeyPressed('1'))
		{
			fsm->changeState(EMS_ROLL);
		}
		else if (CSysUtility::isKeyPressed('2'))
		{
			fsm->changeState(EMS_TORPEDO);
		}
		else if (CSysUtility::isKeyPressed('3'))
		{
			fsm->changeState(EMS_SPINNINGKICK);
		}
	}
}

void CPlayer::onPlayerMoveAction( u32 delta, const SMoveControl& moveControl )
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	wow_m2Move* m2move = node->getM2Move();
	if (!fsm || !m2move)
		return;

	E_M2_STATES state = fsm->getCurrentState();
	vector3df moveSpeed(0,0,0);

	//action
	if ((moveControl.front || moveControl.back || moveControl.left || moveControl.right || moveControl.up) &&
		!(moveControl.front && moveControl.back))
	{
		if (state == EMS_JUMP)
		{
			if (isJumpEnd())
			{
				if( moveControl.front )
				{
					fsm->changeState(EMS_RUN, false);
				}
				else if( moveControl.back )
				{
					fsm->changeState(EMS_WALKBACKWARDS, false);
				}
				else if( moveControl.left || moveControl.right)
				{
					if(moveControl.rightMouse)
						fsm->changeState(EMS_RUN, false);
					else
						fsm->changeState(EMS_STAND, false);
				}

				moveSpeed = getMoveVector();
			}
			else if (isAboutLand())
			{
				setNextLandRun();
			}
		}
		else if (!isStateInterruptible(state))
		{
			if (isStateCanMove(state, true))
				moveSpeed = getMoveVector();
		}
		else		//not jump
		{
			if( moveControl.front )
			{
				fsm->changeState(EMS_RUN, false);
			}
			else if( moveControl.back )
			{
				fsm->changeState(EMS_WALKBACKWARDS, false);
			}
			else if( moveControl.left || moveControl.right)
			{
				if(moveControl.rightMouse)
					fsm->changeState(EMS_RUN, false);
				else
					fsm->changeState(EMS_STAND, false);
			}

			moveSpeed = getMoveVector();

			if ( moveControl.up)
			{
				fsm->changeState(EMS_JUMP);
				setJumpDir(moveSpeed);
			}
		}
	}
	else			//”…walkµΩstand
	{
		if (state == EMS_INVALID || (state >= EMS_WALK && state <= EMS_RUN))
		{
			fsm->changeState(EMS_STAND, false);
		}
		else if (isStateCanMove(state, false))
		{
			moveSpeed = getMoveVector();
		}
	}
	
	//∏ﬂ∂»µ˜’˚
	f32 groundHeight = g_Client->getWorld()->getHeightNormal(m2move->getPos().X, m2move->getPos().Z);
	f32 yoffset = m2move->getDestHeight() + groundHeight - m2move->getPos().Y;
	if (yoffset != 0.0f)
	{
		vector3df voffset(0, yoffset, 0);
		m2move->move(voffset);
		TargetCamera->onTargetMove();

		m2move->setDestHeight(m2move->getPos().Y - groundHeight);
	}

	//move
	state = fsm->getCurrentState();
	switch(state)
	{
	case EMS_RUN:
	case EMS_WALK:
	case EMS_WALKBACKWARDS:
		{
			vector3df offset = moveSpeed * (f32)delta;
			m2move->move(offset);
			TargetCamera->onTargetMove();
		}
		break; 
	case EMS_JUMP:
		{
			vector3df offset = getJumpDir() * (f32)delta;
			m2move->move(offset);
			TargetCamera->onTargetMove();
		}
		break;
	case EMS_ROLL:
	case EMS_TORPEDO:
	case EMS_SPINNINGKICK:
		{
			vector3df offset = moveSpeed * (f32)delta;
			m2move->move(offset);
			TargetCamera->onTargetMove();
		}
	default:
		break;
	}
}

vector3df CPlayer::getMoveVector() const
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	if (!fsm)
		return vector3df::Zero();

	E_M2_STATES state = fsm->getCurrentState();
	vector3df moveDir = node->getM2Move()->getDir();
	switch(state)
	{
	case EMS_RUN:
	case EMS_SPINNINGKICK:
		return moveDir * RunSpeed;
	case EMS_WALK:
		return moveDir * WalkSpeed;
	case EMS_WALKBACKWARDS:
		return -moveDir * BackwardsSpeed;
	case EMS_ROLL:
	case EMS_TORPEDO:
		return moveDir * RollSpeed;
	default:
		return vector3df::Zero();
	}
}

void CPlayer::onPlayerDirection( u32 delta, const SMoveControl& moveControl )
{
	if (moveControl.rightMouse)
		TargetCamera->makeTargetFollowCamera(delta, moveControl.front, moveControl.back, moveControl.left, moveControl.right);
	else
		TargetCamera->makeCameraFollowTarget(delta, moveControl.front, moveControl.back, moveControl.left, moveControl.right);
}

void CPlayer::setMoveSpeed( f32 walk, f32 run, f32 backwards, f32 roll )
{
	WalkSpeed = walk;
	RunSpeed = run;
	BackwardsSpeed = backwards;
	RollSpeed = roll;
}

void CPlayer::getMoveSpeed( f32& walk, f32& run, f32& backwards, f32& roll )
{
	walk = WalkSpeed;
	run = RunSpeed;
	backwards = BackwardsSpeed;
	roll = RollSpeed;
}

bool CPlayer::isJumpEnd() const
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	if (!fsm)
		return false;

	E_M2_STATES state = fsm->getCurrentState();
	if (state != EMS_JUMP)
		return false;

	wow_m2State_Jump* stateJump = static_cast<wow_m2State_Jump*>(fsm->getState(state));
	return stateJump->isJumpEnd();
}

bool CPlayer::isAboutLand() const
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	if (!fsm)
		return false;

	E_M2_STATES state = fsm->getCurrentState();
	if (state != EMS_JUMP)
		return false;

	wow_m2State_Jump* stateJump = static_cast<wow_m2State_Jump*>(fsm->getState(state));
	return stateJump->isAboutLand();
}

void CPlayer::setNextLandRun()
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	if (!fsm)
		return;

	E_M2_STATES state = fsm->getCurrentState();
	if (state != EMS_JUMP)
		return;

	wow_m2State_Jump* stateJump = static_cast<wow_m2State_Jump*>(fsm->getState(state));
	stateJump->setNextLandRun(true);
}

void CPlayer::setJumpDir(const vector3df& dir)
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	if (!fsm)
		return;

	E_M2_STATES state = fsm->getCurrentState();
	if (state != EMS_JUMP)
		return;

	wow_m2State_Jump* stateJump = static_cast<wow_m2State_Jump*>(fsm->getState(state));
	stateJump->setJumpDir(dir);
}

vector3df CPlayer::getJumpDir() const
{
	IM2SceneNode* node = TargetCamera->getM2Target();
	wow_m2FSM* fsm = node->getM2FSM();
	if (!fsm)
		return vector3df::Zero();

	E_M2_STATES state = fsm->getCurrentState();
	if (state != EMS_JUMP)
		return vector3df::Zero();

	wow_m2State_Jump* stateJump = static_cast<wow_m2State_Jump*>(fsm->getState(state));
	return stateJump->getJumpDir();
}

bool CPlayer::isStateCanMove(E_M2_STATES state, bool onKey) const
{
	if (state == EMS_TORPEDO)
		return true;

	if (state == EMS_ROLL)
	{
		IM2SceneNode* node = TargetCamera->getM2Target();
		wow_m2FSM* fsm = node->getM2FSM();
		if (fsm)
		{
			u32 idx = fsm->getDynAction().currentIndex;
			if (idx == 1)
				return true;
			else if (idx == 0)
			{
				animation anim;
				node->getCurrentAnimation(&anim);
				return anim.getCurrentFrame() > anim.getFrameLength() * 0.3f;
			}
			else
			{
				animation anim;
				node->getCurrentAnimation(&anim);
				return anim.getCurrentFrame() < anim.getFrameLength() * 0.2f;
			}
		}
	}

	if (onKey)
	{
		return state == EMS_SPINNINGKICK;
	}

	return false;
}

bool CPlayer::isStateInterruptible(E_M2_STATES state) const
{
	if (state == EMS_TORPEDO || state == EMS_SPINNINGKICK)
		return false;

	if (state == EMS_ROLL)
	{
		IM2SceneNode* node = TargetCamera->getM2Target();
		wow_m2FSM* fsm = node->getM2FSM();
		if (fsm)
		{
			u32 idx = fsm->getDynAction().currentIndex;
			if (idx > 1)
				return true;
		}
		return false;
	}

	return true;
}


