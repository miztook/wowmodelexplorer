#include "stdafx.h"
#include "CCoordSceneNode.h"
#include "mywow.h"

#define NUM_ARROW_POINTS	6

CCoordSceneNode::CCoordSceneNode( ISceneNode* parent )
	: ICoordSceneNode(parent), SelectedAxis(ESA_NONE),
	ColorX(SColor::Red()), ColorY(SColor::Green()), ColorZ(SColor::Blue()), Position2D(EP2D_NONE)
{
	Distance = 18;
	ArrowLength = 0.2f;
	ArrowRadius = 0.05f;
	VisibleX = VisibleY = VisibleZ = true;
}

CCoordSceneNode::~CCoordSceneNode()
{
	
}

void CCoordSceneNode::registerSceneNode( bool frustumcheck, int sequence  )
{
	update(false);

	if (!Visible ||
		(frustumcheck && !isNodeEligible()))
		return;

	g_Engine->getSceneManager()->registerNodeForRendering(this, true, sequence);
}

void CCoordSceneNode::tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible)
{
	if (Position2D != EP2D_NONE)
	{
		const recti& screenSize = g_Engine->getDriver()->getViewPort();
		ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
				
		vector2di pos2d = screenSize.UpperLeftCorner;

		switch(Position2D)
		{
		case EP2D_TOPLEFT:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.1f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.1f);
			}
			break;
		case EP2D_TOPRIGHT:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.9f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.1f);
			}
			break;
		case EP2D_BOTTOMLEFT:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.1f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.9f);
			}
			break;
		case EP2D_BOTTOMRIGHT:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.9f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.9f);
			}
			break;
		case EP2D_TOPCENTER:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.5f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.1f);
			}
			break;
		case EP2D_BOTTOMCENTER:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.5f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.9f);
			}
			break;
		case EP2D_CENTERLEFT:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.1f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.5f);
			}
			break;
		case EP2D_CENTERRIGHT:
			{
				pos2d.X += (s32)(screenSize.getWidth() * 0.9f);
				pos2d.Y += (s32)(screenSize.getHeight() * 0.5f);
			}
			break;
        default:
            break;
		}
		vector3df pos = cam->get3DPositionFromScreenPosition(pos2d, screenSize, Distance);

		RelativeTransformation.setTranslation(pos);
		NeedUpdate = true;
		update(false);
	}	
	
	calculateAxisParam(AxisParam);
}

void CCoordSceneNode::render() const
{
	drawAxis(AxisParam);

	drawArrows(AxisParam);

	drawText(AxisParam);
}

void CCoordSceneNode::checkSelectedAxis( vector2di pos )
{

}

bool CCoordSceneNode::intersectWith( vector2di pos, E_AXIS axis ) const
{

	return false;
}

void CCoordSceneNode::calculateAxisParam(SAxisParam& param)
{
	param.center = vector3df(0,0,0);
	param.xPos = vector3df(Length, 0, 0);
	param.yPos = vector3df(0, Length, 0);
	param.zPos = vector3df(0, 0, Length);

	AbsoluteTransformation.transformVect(param.center);
	AbsoluteTransformation.transformVect(param.xPos);
	AbsoluteTransformation.transformVect(param.yPos);
	AbsoluteTransformation.transformVect(param.zPos);

	param.xDir = (param.xPos - param.center).normalize();
	param.yDir = (param.yPos - param.center).normalize();
	param.zDir = (param.zPos - param.center).normalize();
}

void CCoordSceneNode::drawArrows(const SAxisParam& param) const
{
	vector3df vscale = AbsoluteTransformation.getScale();
	f32 scale = vscale.getLength();

	//arrow
	const f32 arrowLength = Length * ArrowLength * scale;
	const f32 arrowWidth = Length * ArrowRadius * scale;
	const f32 angle = 2 * PI / NUM_ARROW_POINTS;
	u32 vcount = NUM_ARROW_POINTS + 2;
	u32 icount = NUM_ARROW_POINTS * 6;
	vector3df* vertices = (vector3df*)Z_AllocateTempMemory(sizeof(vector3df) * vcount);
	u16* indices = (u16*)Z_AllocateTempMemory(sizeof(u16) * icount);

	if (VisibleX)
	{
		vector3df center = param.xPos - param.xDir * arrowLength;

		for (u32 i=0; i<NUM_ARROW_POINTS; ++i)
		{
			vector3df v = center + param.yDir * arrowWidth * cos(angle * i) +  param.zDir * arrowWidth * sin(angle * i);
			vertices[i] = v;
		}
		vertices[NUM_ARROW_POINTS] = center;
		vertices[NUM_ARROW_POINTS+1] = param.xPos;

		for (u32 i=0; i<NUM_ARROW_POINTS; ++i)
		{
			indices[i*6] = i;
			indices[i*6+1] = (i == (NUM_ARROW_POINTS-1) ? 0 : (i+1));
			indices[i*6+2] = NUM_ARROW_POINTS;

			indices[i*6+3] = (i == (NUM_ARROW_POINTS-1) ? 0 : (i+1));
			indices[i*6+4] = i;
			indices[i*6+5] = NUM_ARROW_POINTS+1;
		}

		g_Engine->getDrawServices()->add3DVertices(vertices, vcount, indices, icount, ColorX);
	}

	if (VisibleY)
	{
		vector3df center = param.yPos - param.yDir * arrowLength;

		for (u32 i=0; i<NUM_ARROW_POINTS; ++i)
		{
			vector3df v = center + param.xDir * arrowWidth * cos(angle * i) +  param.zDir * arrowWidth * sin(angle * i);
			vertices[i] = v;
		}
		vertices[NUM_ARROW_POINTS] = center;
		vertices[NUM_ARROW_POINTS+1] = param.yPos;

		for (u32 i=0; i<NUM_ARROW_POINTS; ++i)
		{
			indices[i*6] = i;
			indices[i*6+1] = (i == (NUM_ARROW_POINTS-1) ? 0 : (i+1));
			indices[i*6+2] = NUM_ARROW_POINTS;

			indices[i*6+3] = (i == (NUM_ARROW_POINTS-1) ? 0 : (i+1));
			indices[i*6+4] = i;
			indices[i*6+5] = NUM_ARROW_POINTS+1;
		}

		g_Engine->getDrawServices()->add3DVertices(vertices, vcount, indices, icount, ColorY);
	}
	
	if (VisibleZ)
	{
		vector3df center = param.zPos - param.zDir * arrowLength;

		for (u32 i=0; i<NUM_ARROW_POINTS; ++i)
		{
			vector3df v = center + param.xDir * arrowWidth * cos(angle * i) +  param.yDir * arrowWidth * sin(angle * i);
			vertices[i] = v;
		}
		vertices[NUM_ARROW_POINTS] = center;
		vertices[NUM_ARROW_POINTS+1] = param.zPos;

		for (u32 i=0; i<NUM_ARROW_POINTS; ++i)
		{
			indices[i*6] = i;
			indices[i*6+1] = (i == (NUM_ARROW_POINTS-1) ? 0 : (i+1));
			indices[i*6+2] = NUM_ARROW_POINTS;

			indices[i*6+3] = (i == (NUM_ARROW_POINTS-1) ? 0 : (i+1));
			indices[i*6+4] = i;
			indices[i*6+5] = NUM_ARROW_POINTS+1;
		}

		g_Engine->getDrawServices()->add3DVertices(vertices, vcount, indices, icount, ColorZ);
	}

	Z_FreeTempMemory(indices);
	Z_FreeTempMemory(vertices);
}

void CCoordSceneNode::drawAxis(const SAxisParam& param) const
{
	if (VisibleX)
		g_Engine->getDrawServices()->add3DLine(line3df(param.center, param.xPos), ColorX);
	
	if (VisibleY)
		g_Engine->getDrawServices()->add3DLine(line3df(param.center, param.yPos), ColorY);
	
	if (VisibleZ)
		g_Engine->getDrawServices()->add3DLine(line3df(param.center, param.zPos), ColorZ);
}

void CCoordSceneNode::drawText(const SAxisParam& param) const
{
	f32 len = Length * 1.4f;
	vector3df posX(len, 0, 0);
	vector3df posY(0, len, 0);
	vector3df posZ(0, 0, len);

	const recti& screenSize = g_Engine->getDriver()->getViewPort();
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();

	if (VisibleX)
	{
		AbsoluteTransformation.transformVect(posX);
		vector2di xPos = cam->getScreenPositionFrom3DPosition(posX, screenSize);

		if (TextX.length())
			g_Engine->getDefaultFont()->addTextA(TextX.c_str(), ColorX, xPos);
	}
	
	if (VisibleY)
	{
		AbsoluteTransformation.transformVect(posY);
		vector2di yPos = cam->getScreenPositionFrom3DPosition(posY, screenSize);

		if (TextY.length())
			g_Engine->getDefaultFont()->addTextA(TextY.c_str(), ColorY, yPos);
	}
	
	if (VisibleZ)
	{
		AbsoluteTransformation.transformVect(posZ);
		vector2di zPos = cam->getScreenPositionFrom3DPosition(posZ, screenSize);

		if (TextZ.length())
			g_Engine->getDefaultFont()->addTextA(TextZ.c_str(), ColorZ, zPos);
	}
}

void CCoordSceneNode::setDir( E_AXIS axis, const vector3df& dir )
{
	vector3df from, oppoAxis;
	switch (axis)
	{
	case ESA_X:
		from = vector3df::UnitX();
		oppoAxis = vector3df::UnitY();
		break;
	case ESA_Y:
		from = vector3df::UnitY();
		oppoAxis = vector3df::UnitX();
		break;
	case ESA_Z:
		from = vector3df::UnitZ();
		oppoAxis = vector3df::UnitY();
		break;
	default:
		return;
	}
	quaternion q;
	q.rotationFromTo(from, dir, oppoAxis);
	matrix4 mat;
	q.getMatrix(mat);
	RelativeTransformation = mat;
	NeedUpdate = true;
	update(false);

	calculateAxisParam(AxisParam);
}

vector3df CCoordSceneNode::getDir( E_AXIS axis ) const
{
	switch (axis)
	{
	case ESA_X:
		return AxisParam.xDir;
	case ESA_Y:
		return AxisParam.yDir;
	case ESA_Z:
		return AxisParam.zDir;
	default:
		ASSERT(false);
		return vector3df::Zero();
	}
}

void CCoordSceneNode::pitch_yaw_FPS( f32 pitchDegree, f32 yawDegree )
{
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();

	quaternion quatX(pitchDegree * DEGTORAD, cam->getRight());			
	quaternion quatY(yawDegree * DEGTORAD, vector3df::UnitY());
	quaternion q = (quatX * quatY);

	matrix4 mat;
	q.getMatrix(mat);
	RelativeTransformation = RelativeTransformation * mat;
	NeedUpdate = true;
	update(false);
}

