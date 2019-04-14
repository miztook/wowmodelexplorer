// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

#ifndef _COMMON
#define _COMMON

half CalcFogFactor( half d, half4 fogParams )
{
	half fogStart = fogParams[1];
	half fogEnd = fogParams[2];
	half fogDensity = fogParams[3];
	
	half fogCoeff = clamp((d - fogStart) / (fogEnd - fogStart), 0.0f, 1.0f);
	fogCoeff = pow(fogCoeff, fogDensity);

	return fogCoeff;
}

#endif