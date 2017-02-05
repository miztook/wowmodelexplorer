// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

sampler2D	g_TexSampler0;

struct PS_INPUT
{
	half4 Col0 : COLOR0;
	half2 Tex0 : TEXCOORD0;
};

float4 main( PS_INPUT i ) : COLOR0
{
	float4 col;
	float4 texCol = tex2D(g_TexSampler0, i.Tex0);
	col.rgb = texCol.rgb * i.Col0.rgb;

	col.a = 1.0f;
	
	return col;
}