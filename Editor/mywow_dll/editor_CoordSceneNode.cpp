#include "stdafx.h"
#include "editor_CoordSceneNode.h"

void CoordSceneNode_setPosition2D( ICoordSceneNode* node, ICoordSceneNode::E_POSITION_2D position, float distance )
{
	node->setPosition2D(position, distance);
}

void CoordSceneNode_setAxisColor( ICoordSceneNode* node, SColor colorX, SColor colorY, SColor colorZ )
{
	node->setAxisColor(colorX, colorY, colorZ);
}

void CoordSceneNode_setAxisText( ICoordSceneNode* node, const char* textX, const char* textY, const char* textZ )
{
	node->setAxisText(textX, textY, textZ);
}

void CoordSceneNode_setAxisVisible( ICoordSceneNode* node, bool xVisible, bool yVisible, bool zVisible )
{
	node->setAxisVisible(xVisible, yVisible, zVisible);
}

void CoordSceneNode_setDir( ICoordSceneNode* node, ICoordSceneNode::E_AXIS axis, vector3df dir )
{
	node->setDir(axis, dir);
}

void CoordSceneNode_getDir( ICoordSceneNode* node, ICoordSceneNode::E_AXIS axis, vector3df* dir )
{
	*dir = node->getDir(axis);
}

void CoordSceneNode_pitch_yaw_FPS( ICoordSceneNode* node, float pitchDegree, float yawDegree )
{
	node->pitch_yaw_FPS(pitchDegree, yawDegree);
}