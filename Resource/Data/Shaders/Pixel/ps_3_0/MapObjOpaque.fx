// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
struct ConstantBuffer
{
	half4 		AmbientColor;
	half4 		FogColor;
};

ConstantBuffer g_cbuffer : register(c0);

sampler2D	g_TexSampler0;

struct PS_INPUT
{
	half4 Diffuse : COLOR0;
	half3 Tex0 : TEXCOORD0;		//z: fog alpha		
};

float4 main( PS_INPUT i ) : COLOR0
{
	float fogAlpha = i.Tex0.z;
	
	float4 color = tex2D(g_TexSampler0, i.Tex0);
	
	color = color * i.Diffuse;		
	color.a = 1.0f;	
	
	//fog
	color.rgb = color.rgb * (1.0f-fogAlpha) + g_cbuffer.FogColor.rgb * fogAlpha;
	
	return color;
}