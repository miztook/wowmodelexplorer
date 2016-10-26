#pragma once

#include "core.h"
#include "uibase.h"

class uiControl
{
public:
	uiControl(): Type(ControlType_Unknown) {}

public:
	virtual E_CONTROL_TYPE getType() const { return Type; }
	virtual void registerSceneNode(bool screenClip = true);
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible) {}
	virtual void render() = 0;

	const recti& getWorldBoundingRect() const { return BoundingRect; }

public:
	vector2di pointToScreen(const vector2di& point);
	vector2di pointToClient(const vector2di& point);

	vector2di getFramePoint(E_FRAME_POINT framepoint);

public:
	E_CONTROL_TYPE Type;
	recti	BoundingRect;
	vector2di	Location;
	dimension2du Size;
	
};