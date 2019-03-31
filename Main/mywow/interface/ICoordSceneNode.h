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
	virtual void setPosition2D(E_POSITION_2D pos2d, float distance = 18) = 0;
	virtual void setAxisVisible(bool xVisible, bool yVisible, bool zVisible) = 0;
	virtual void setAxisColor(SColor colorX, SColor colorY, SColor colorZ) = 0;
	virtual void setAxisText(const char* textX, const char* textY, const char* textZ) = 0;
	virtual void setArrowSize(float length, float radius) = 0;
	virtual void setDir(E_AXIS axis, const vector3df& dir) = 0;
	virtual vector3df getDir(E_AXIS axis) const = 0;
	virtual void pitch_yaw_FPS(float pitchDegree, float yawDegree) = 0;

public:	
	float  Length;
	E_COORD_MODE		Mode;
};