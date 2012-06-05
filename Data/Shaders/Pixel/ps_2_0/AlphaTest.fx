// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
struct ConstantBuffer
{
	float4	Params;		//0: alphaSale
};

ConstantBuffer g_cbuffer : register(c0);

sampler2D g_TexSampler = sampler_state
{
};

float ScaleAlpha(float alpha)
{
	return alpha * g_cbuffer.Params[0];
}

struct PS_IN
{
	float4 Pos : POSITION;
	float4 Diffuse : COLOR0;
	float2 Tex : TEXCOORD0;
};

float4 main(PS_IN i) : COLOR
{
	float4 color = tex2D(g_TexSampler, i.Tex) * i.Diffuse;
	
	if(color.a < ScaleAlpha(0.8f))
		clip(-1.0f);
	else
		color.a = ScaleAlpha(1.0f);
	
	return color;
}