// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
struct ConstantBuffer
{
	float4	Params;		//0: numTextures
};

ConstantBuffer g_cbuffer : register(c0);

sampler2D g_TexSampler0;
sampler2D g_TexSampler1;
sampler2D g_TexSampler2;
sampler2D g_TexSampler3;
sampler2D g_TexSampler4;

struct PS_IN
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex0 : TEXCOORD0;		//base texture
	float2 Tex1 : TEXCOORD1;		//alpha map 
};

float4 main(PS_IN i) : COLOR
{
	int numTextures = (int)g_cbuffer.Params[0];
	
	float4 color = tex2D(g_TexSampler0, i.Tex0);
	float4 alpha = tex2D(g_TexSampler4, i.Tex1);
	float4 specular = float4(0.5f, 0.5f, 0.5f, 1.0f);
	//layer1
	if(numTextures > 1)
	{
		float4 color1 = tex2D(g_TexSampler1, i.Tex0); 
		color = color1 * alpha.b + color * (1-alpha.b);
	}
	
	//layer2
	if(numTextures > 2)
	{
		float4 color2 = tex2D(g_TexSampler2, i.Tex0); 
		color = color2 * alpha.g + color * (1-alpha.g);
	}
	
	//layer3
	if(numTextures > 3)
	{
		float4 color3 = tex2D(g_TexSampler3, i.Tex0); 
		color = color3 * alpha.r + color * (1-alpha.r);
	}
	
	//blend
	float x = (1.0f - alpha.a) * (color.a * 0.3f + 0.69999999f);	
	color = color * x + color.a * specular;	
	//color.r = color.g = color.b = color.a;
	color.a = 1.0f;
	
	return color;
}