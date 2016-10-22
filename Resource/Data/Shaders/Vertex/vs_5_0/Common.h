// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

#ifndef _COMMON
#define _COMMON

float CalcFogFactor( float d, float4 fogParams )
{
	float fogStart = fogParams[1];
	float fogEnd = fogParams[2];
	float fogDensity = fogParams[3];
	
	float fogCoeff = clamp((d - fogStart) / (fogEnd - fogStart), 0.0f, 1.0f);
	fogCoeff = pow(fogCoeff, fogDensity);

	return fogCoeff;
}

#endif