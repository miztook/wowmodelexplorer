// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

struct PS_INPUT
{
	half4 Col0 : COLOR0;
};

float4 main( PS_INPUT i ) : COLOR0
{
	return i.Col0;
}