#pragma once

#include "base.h"
#include "core.h"
#include "SColor.h"

#define  MAX_DLIGHT_RADIUS	10.0f

struct SLight
{
	SLight() : LightColor(255,255,255,255),
		Attenuation(1.f,0.f,0.f),
		Position(0.f,0.f,0.f), Direction(0.f,0.f,1.f),
		OuterCone(45.f), InnerCone(0.f), Falloff(2.f),
		Radius(100.f), Type(ELT_POINT)
	{
	}

	//
	SColor	LightColor;

	vector3df	Attenuation;
	vector3df	Position;
	vector3df	Direction;
	float	OuterCone;
	float	InnerCone;
	float	Falloff;
	float	Radius;
	E_LIGHT_TYPE		Type;
};