// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

sampler2D	g_TexSampler0;

struct PS_INPUT
{
	half4 Col0 : COLOR0;
	half2 Tex0 : TEXCOORD0;
};

float4 main( PS_INPUT i ) : COLOR0
{
	
	return tex2D(g_TexSampler0, i.Tex0) * i.Col0;
}