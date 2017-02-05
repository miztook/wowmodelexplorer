// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

struct ConstantBuffer
{
	float4x4 	mWorldViewProjection;    // World * View * Projection matrix
	half4		vColor;
	half4		Params;				//0: numBones, 1: animTexture
	half4x4	mTexture;
};

ConstantBuffer g_cbuffer : register(c0);

struct VS_INPUT
{
    float4  Pos             : POSITION;
	half2  Tex0            : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
	half4 Diffuse : COLOR0;
	half2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};

VS_OUTPUT main( VS_INPUT i )
{
	VS_OUTPUT Output;
	
	float3	Tex = float3(i.Tex0.xy, 1.0f);

	int bAnimTexture = (int)g_cbuffer.Params[1];
	
	Output.Position = mul(g_cbuffer.mWorldViewProjection, float4(i.Pos.xyz, 1.0f));
	Output.Diffuse = g_cbuffer.vColor;
	if (bAnimTexture)
	{
		Tex = (float3)mul(g_cbuffer.mTexture, float4(Tex, 1.0f));
		Output.TextureUV = Tex.xy; 
	}
	else
	{
		Output.TextureUV = i.Tex0.xy; 
	}
	
	return Output;
}