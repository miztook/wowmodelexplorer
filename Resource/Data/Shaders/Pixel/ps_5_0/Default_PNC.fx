// <effectEd direct3D="10" profile="fx_4_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

//#define _ALPHATEST_

struct ConstantBuffer
{
	float4 		Params;		//0: alphatest, 1: ref
};

cbuffer cb0 : register(b0)
{
	ConstantBuffer g_cbuffer;
}

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Col0 : COLOR0;
};

float4 main( PS_INPUT i ) : SV_Target
{
	int alphaTest = (int)g_cbuffer.Params[0];
	float alphaRef = g_cbuffer.Params[1];
	
	float4 color = i.Col0;
	
#ifdef _ALPHATEST_
	if(alphaTest & color.a < alphaRef)
		clip(-1.0f);
#endif
		
	return color;
}