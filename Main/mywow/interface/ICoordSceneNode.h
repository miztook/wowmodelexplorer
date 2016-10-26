#pragma once

#include "ISceneNode.h"
#include "SColor.h"

class ICoordSceneNode : public ISceneNode
{
public:
	enum E_COORD_MODE : int32_t
	{
		ECM_MOVE = 0,
		ECM_ROTATE,
		ECM_SCALE,
		ECM_SCALEALL,
	};

	enum E_AXIS : int32_t
	{
		ESA_NONE = 0,
		ESA_X,
		ESA_Y,
		ESA_Z,
	};

	enum E_POSITION_2D : int32_t
	{
		EP2D_NONE = 0,
		EP2D_TOPLEFT,
		EP2D_TOPRIGHT,
		EP2D_BOTTOMLEFT,
		EP2D_BOTTOMRIGHT,
		EP2D_TOPCENTER,
		EP2D_BOTTOMCENTER,
		EP2D_CENTERLEFT,
		EP2D_CENTERRIGHT,
	};

public:
	explicit ICoordSceneNode(ISceneNode* parent) : ISceneNode(parent),
		Mode(ECM_MOVE), Length(1.0f) { Type = EST_COORD; }
	virtual ~ICoordSceneNode() {}

public:
	virtual E_AXIS getSelectedAxis() const = 0;
	virtual void checkSelectedAxis(vector2di pos) = 0;
	virtual void setPosition2D(E_POSITION_2D pos2d, f32 distance = 18) = 0;
	virtual void setAxisVisible(bool xVisible, bool yVisible, bool zVisible) = 0;
	virtual void setAxisColor(SColor colorX, SColor colorY, SColor colorZ) = 0;
	virtual void setAxisText(const c8* textX, const c8* textY, const c8* textZ) = 0;
	virtual void setArrowSize(f32 length, f32 radius) = 0;
	virtual void setDir(E_AXIS axis, const vector3df& dir) = 0;
	virtual vector3df getDir(E_AXIS axis) const = 0;
	virtual void pitch_yaw_FPS(f32 pitchDegree, f32 yawDegree) = 0;

public:	
	f32  Length;
	E_COORD_MODE		Mode;
};