// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

#include "Common.h"

struct ConstantBuffer
{
	float4x4 	mWorldViewProjection;    // World * View * Projection matrix
	float4x4	mWorldView;			// World * View matrix
	half4		vColor;
	half4  	FogParams;		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
	half4		Params;				//0: numBones, 1: animTexture, 2: fogEnable
	half4x4		mTexture;
};

ConstantBuffer g_cbuffer : register(c0);

struct VS_INPUT
{
    float4  Pos            : POSITION;
	half4	Col0 		   : COLOR0;
	half2  Tex0            : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
	half4  Diffuse   : COLOR0;
	half3 TextureUV  : TEXCOORD0;  // vertex texture coords, z: fog alpha
};

VS_OUTPUT main( VS_INPUT i )
{
    VS_OUTPUT Output;
  
	float3  Pos = (float3)i.Pos;  
	float3	Tex = float3(i.Tex0.xy, 1.0f);
	
	int bAnimTexture = (int)g_cbuffer.Params[1];
	int bEnableFog = (int)g_cbuffer.Params[2];
	
	Output.Position = mul(g_cbuffer.mWorldViewProjection, float4(Pos.xyz, 1.0f));
	Output.Diffuse = g_cbuffer.vColor * i.Col0;
	
	if (bAnimTexture)
	{
		Tex = (float3)mul(g_cbuffer.mTexture, float4(Tex, 1.0f));
		Output.TextureUV.xy = Tex.xy; 
	}
	else
	{
		Output.TextureUV.xy = i.Tex0.xy; 
	}
	
	if(bEnableFog)
	{
		float4 cameraPos = mul(g_cbuffer.mWorldView, float4(i.Pos.xyz, 1.0f));
		Output.TextureUV.z = CalcFogFactor(cameraPos.z, g_cbuffer.FogParams);
	}
	else
	{
		Output.TextureUV.z = 0.0f;
	}
			
    return Output;    
}

