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
sampler2D	g_TexSampler1;

struct PS_INPUT
{
	half4 Col0 : COLOR0;
	half3 Tex0 : TEXCOORD0;
	half2 Tex1 : TEXCOORD1;
};

float4 main( PS_INPUT i ) : COLOR0
{
	float4 col = tex2D(g_TexSampler0, i.Tex0) * i.Col0;
	col = col * tex2D(g_TexSampler1, i.Tex1);
	col = col * 2;

	//fog
	float fogAlpha = i.Tex0.z;
	col.rgb = col.rgb * (1.0f-fogAlpha) + g_cbuffer.FogColor.rgb * fogAlpha;
	
	return col;
}