#include "Common.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

struct ConstantBuffer
{
	float4x4 	mWorldViewProjection;    // World * View * Projection matrix
	float4x4	mWorldView;			// World * View matrix
	float4		LightDir;			// light dir in world space
	float4		LightColor;			// light color;
	half4		MaterialParams;		//0: ambient, 1: diffuse, 2: specular
	half4  	FogParams;	//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
	half4		Params;				//0: animTexture, 1: FogEnable
	half4x4	mTexture;
};

ConstantBuffer g_cbuffer : register(c0);

struct VS_INPUT
{
    float4  Pos             : POSITION;
    half3 	Normal			: NORMAL;
	half4 	Col0			: COLOR0;
	half2  Tex0            : TEXCOORD0;
	half2	Tex1			: TEXCOORD1;
};

struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
	half4  Diffuse   : COLOR0;
	half4  Specular	 : COLOR1;
	half3 TextureUV  : TEXCOORD0;  // vertex texture coords z: fog alpha 
};


VS_OUTPUT main( VS_INPUT i )
{
	VS_OUTPUT Output;
	
	float ambient = g_cbuffer.MaterialParams[0];
	float diffuse = g_cbuffer.MaterialParams[1];
	float specular = g_cbuffer.MaterialParams[2];
	
	int bAnimTexture = (int)g_cbuffer.Params[0];
	int bEnableFog = (int)g_cbuffer.Params[1];
	
	float3 Tex = float3(i.Tex0, 0);
		
	Output.Position = mul(g_cbuffer.mWorldViewProjection, float4(i.Pos.xyz, 1.0f));
	float3 cameraPos = (float3)mul(g_cbuffer.mWorldView, float4(i.Pos.xyz, 1.0f));
	
	float3 lightDir = g_cbuffer.LightDir.xyz;
	float3 normal = mul(g_cbuffer.mWorldView, i.Normal.xyz);
	normal = normalize(normal);
	Output.Specular	= float4(specular, specular, specular, 1.0f);
	
	Output.Diffuse.rgb = g_cbuffer.LightColor * clamp(dot(normal , -lightDir), 0.0f, 1.0f) * diffuse + ambient; 
	Output.Diffuse.a = i.Col0.a;
	
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
		Output.TextureUV.z = CalcFogFactor(cameraPos.z, g_cbuffer.FogParams);
	else
		Output.TextureUV.z = 0.0f;
	
	return Output;
}