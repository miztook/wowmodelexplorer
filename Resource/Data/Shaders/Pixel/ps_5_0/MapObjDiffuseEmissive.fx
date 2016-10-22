// <effectEd direct3D="10" profile="fx_4_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

//#define _ALPHATEST_

struct ConstantBuffer
{
	float4		AmbientColor;
	float4 		FogColor;
	float4 		Params;		//0: alphatest, 1: ref
};

cbuffer cb0 : register(b0)
{
	ConstantBuffer g_cbuffer;
}

SamplerState	g_TexSampler0;

Texture2D	g_Tex0;

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Diffuse : COLOR0;
	float3 Tex0 : TEXCOORD0;	//z: fog alpha		
};

float4 main( PS_INPUT i ) : SV_Target
{
	float fogAlpha = i.Tex0.z;
	int alphaTest = (int)g_cbuffer.Params[0];
	float alphaRef = g_cbuffer.Params[1];
	
	float4 color;
	
	color = g_Tex0.Sample(g_TexSampler0, i.Tex0);

	color.rgb 	= color.rgb * i.Diffuse * 1.5f;
	
#ifdef _ALPHATEST_
	if(alphaTest & color.a < alphaRef)
		clip(-1.0f);
#endif
		
	//fog
	color.rgb = color.rgb * (1.0f - fogAlpha) + g_cbuffer.FogColor.rgb * fogAlpha;
	
	return color;
}