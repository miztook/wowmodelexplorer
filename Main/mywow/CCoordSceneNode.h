#pragma once

#include "ICoordSceneNode.h"
#include "core.h"

class CCoordSceneNode : public ICoordSceneNode
{
public:
	explicit CCoordSceneNode(ISceneNode* parent);
	~CCoordSceneNode();

public:
	//ICoordSceneNode
	virtual E_AXIS getSelectedAxis() const { return SelectedAxis; }
	virtual void checkSelectedAxis(vector2di pos);
	virtual void setPosition2D(E_POSITION_2D pos2d, float distance = 18);
	virtual void setAxisVisible(bool xVisible, bool yVisible, bool zVisible);
	virtual void setAxisColor(SColor colorX, SColor colorY, SColor colorZ);
	virtual void setAxisText(const char* textX, const char* textY, const char* textZ);
	virtual void setArrowSize(float length, float radius);
	virtual void setDir(E_AXIS axis, const vector3df& dir);
	virtual vector3df getDir(E_AXIS axis) const;
	virtual void pitch_yaw_FPS(float pitchDegree, float yawDegree);

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const { return aabbox3df::Zero(); }
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const { return Visible; }

private:
	struct SAxisParam 
	{
		SAxisParam() 
		{
			center.clear();
			xPos = xDir = vector3df::UnitX();
			yPos = yDir = vector3df::UnitY();
			zPos = zDir = vector3df::UnitZ();
		}
		vector3df center;
		vector3df xPos;
		vector3df yPos;
		vector3df zPos;
		vector3df xDir;
		vector3df yDir;
		vector3df zDir;
	};

	SAxisParam	AxisParam;

	bool intersectWith(vector2di pos, E_AXIS axis) const;
	void calculateAxisParam(SAxisParam& param);
	void drawArrows(const SAxisParam& param) const;
	void drawAxis(const SAxisParam& param) const;
	void drawText(const SAxisParam& param) const;

private:
	float		ArrowLength, ArrowRadius;
	E_AXIS		SelectedAxis;
	E_POSITION_2D	Position2D;

	SColor ColorX, ColorY, ColorZ;
	string16 TextX, TextY, TextZ;
	bool VisibleX, VisibleY, VisibleZ;
};

inline void CCoordSceneNode::setPosition2D( E_POSITION_2D pos2d, float distance )
{
	Position2D = pos2d;
	Distance = distance;
}

inline void CCoordSceneNode::setAxisVisible( bool xVisible, bool yVisible, bool zVisible )
{
	VisibleX = xVisible;
	VisibleY = yVisible;
	VisibleZ = zVisible;
}

inline void CCoordSceneNode::setAxisColor( SColor colorX, SColor colorY, SColor colorZ )
{
	ColorX = colorX;
	ColorY = colorY;
	ColorZ = colorZ;
}

inline void CCoordSceneNode::setAxisText( const char* textX, const char* textY, const char* textZ )
{
	TextX = textX;
	TextY = textY;
	TextZ = textZ;
}

inline void CCoordSceneNode::setArrowSize( float length, float radius )
{
	ArrowLength = length;
	ArrowRadius = radius;
}


