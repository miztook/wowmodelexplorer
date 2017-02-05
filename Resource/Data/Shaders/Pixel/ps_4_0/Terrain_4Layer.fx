// <effectEd direct3D="10" profile="fx_4_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
struct ConstantBuffer
{
	float4	Params;		//0: numTextures,
	float4  FogColor;
};

cbuffer cb0 : register(b0)
{
	ConstantBuffer g_cbuffer;
}

SamplerState	g_TexSampler0;
SamplerState	g_TexSampler1;
SamplerState	g_TexSampler2;
SamplerState	g_TexSampler3;
SamplerState	g_TexSampler4;

Texture2D	g_Tex0;
Texture2D	g_Tex1;
Texture2D	g_Tex2;
Texture2D	g_Tex3;
Texture2D	g_Tex4;

struct PS_IN
{
	float4 Position : SV_Position;
	float4 Diffuse : COLOR0;
	float4 Specular : COLOR1;
	float2 Tex0 : TEXCOORD0;		//base texture
	float3 Tex1 : TEXCOORD1;		//alpha map, z: fogAlpha
};


float4 main(PS_IN i) : SV_Target
{
	float fogAlpha = i.Tex1.z;
	
	float4 alpha = g_Tex0.Sample(g_TexSampler0, i.Tex1.xy);
	float4 color = g_Tex1.Sample(g_TexSampler1, i.Tex0.xy);
	float4 specular = i.Specular;
	
	//layer2
	float4 color1 = g_Tex2.Sample(g_TexSampler2, i.Tex0.xy); 
	color = color1 * alpha.b + color * (1-alpha.b);
	
	//layer3
	float4 color2 = g_Tex3.Sample(g_TexSampler3, i.Tex0.xy); 
	color = color2 * alpha.g + color * (1-alpha.g);
	
	//layer4
	float4 color3 = g_Tex4.Sample(g_TexSampler4, i.Tex0.xy); 
	color = color3 * alpha.r + color * (1-alpha.r);
	
	//blend
	float x = (1.0f - alpha.a) * 0.3f + 0.69999999f;	
	color.rgb = color.rgb * x * i.Diffuse + (1.0f - alpha.a) * color.a * specular.rgb;	
	
	//fog
	color.rgb = color.rgb * (1.0f - fogAlpha) + g_cbuffer.FogColor.rgb * fogAlpha;

	color.a = 1.0f;
	
	return color;
}