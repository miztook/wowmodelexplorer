// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
struct ConstantBuffer
{
	half4 		FogColor;
};

ConstantBuffer g_cbuffer : register(c0);
sampler2D	g_TexSampler0;

struct PS_INPUT
{
	half4 Col0 : COLOR0;
	half3 Tex0 : TEXCOORD0;
};

float4 main( PS_INPUT i ) : COLOR0
{
	float4 col;
	col.rgb	= tex2D(g_TexSampler0, i.Tex0).rgb * i.Col0.rgb;
	col.a = i.Col0.a;
	
	//fog
	float fogAlpha = i.Tex0.z;
	col.rgb = col.rgb * (1.0f-fogAlpha) + g_cbuffer.FogColor.rgb * fogAlpha;
	
	return col;
}