// <effectEd direct3D="10" profile="fx_4_0" shaderFlags="None, WarningsAreErrors" />

SamplerState	g_TexSampler0;

Texture2D	g_Tex0;

struct PS_INPUT
{
	float4 Position : SV_Position;
	float4 Col0 : COLOR0;
	float2 Tex0 : TEXCOORD0;
};

float4 main( PS_INPUT i ) : SV_Target
{
	float4 col;
	float4 texCol = g_Tex0.Sample(g_TexSampler0, i.Tex0);
	col.rgb = texCol.rgb * i.Col0.rgb;

	col.a = texCol.a * i.Col0.a;

	return col;
}