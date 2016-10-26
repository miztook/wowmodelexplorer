#pragma once

#include "base.h"
#include "core.h"
#include "SColor.h"

#define  MAX_DLIGHT_RADIUS	10.0f

struct SLight
{
	SLight() : AmbientColor( 255, 128,0,0 ), DiffuseColor(255,255,255,255),
		SpecularColor(255,255,255,255), Attenuation(1.f,0.f,0.f),
		Position(0.f,0.f,0.f), Direction(0.f,0.f,1.f),
		OuterCone(45.f), InnerCone(0.f), Falloff(2.f),
		Radius(100.f), Type(ELT_POINT)
	{
	}

	//
	SColor	AmbientColor;
	SColor	DiffuseColor;
	SColor	SpecularColor;

	vector3df	Attenuation;
	vector3df	Position;
	vector3df	Direction;
	f32	OuterCone;
	f32	InnerCone;
	f32	Falloff;
	f32	Radius;
	E_LIGHT_TYPE		Type;
};