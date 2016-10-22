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
SamplerState	g_TexSampler1;

Texture2D	g_Tex0;
Texture2D	g_Tex1;

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Diffuse : COLOR0;
	float3 Tex0 : TEXCOORD0;	//z: fog alpha	
	float2 Tex1 : TEXCOORD1;
};

float4 main( PS_INPUT i ) : SV_Target
{
	float fogAlpha = i.Tex0.z;
	int alphaTest = (int)g_cbuffer.Params[0];
	float alphaRef = g_cbuffer.Params[1];
	
	float4 color;

	float4 tex0	= g_Tex0.Sample(g_TexSampler0, i.Tex0);
	float4 tex1 = g_Tex1.Sample(g_TexSampler1, i.Tex1);
	
	color = tex0 * i.Diffuse + i.Diffuse * tex1 * tex0.w;
	
#ifdef _ALPHATEST_
	if(alphaTest & color.a < alphaRef)
		clip(-1.0f);
#endif	
		
	//fog
	color.rgb = color.rgb * (1.0f - fogAlpha) + g_cbuffer.FogColor.rgb * fogAlpha;
	
	return color;
}