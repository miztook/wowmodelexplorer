// <effectEd direct3D="10" profile="fx_4_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

#include "Common.h"

static const int MAX_MATRICES = 58;

struct ConstantBuffer
{
	float4x4 	mWorldViewProjection;    // World * View * Projection matrix
	float4x4	mWorldView;			// World * View matrix
	float4		vColor;
	float4  	FogParams;		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
	float4		Params;				//0: numBones, 1: animTexture
	float4x4	mTexture;
	
	float3x4    mBoneMatrixArray[MAX_MATRICES];			// Transposed for efficiency
};

//const buffer
cbuffer cb0 : register(b0)
{
	ConstantBuffer g_cbuffer;
}

struct VS_INPUT
{
    float4  Pos             : POSITION;
	float3	Normal			: NORMAL;
	float2  Tex0            : TEXCOORD0;
	float2  Tex1			: TEXCOORD1;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
};

struct VS_OUTPUT
{
    float4 Position   : SV_Position;   // vertex position 
	float4  Diffuse   : COLOR0;
	float3 TextureUV  : TEXCOORD0;  // vertex texture coords, z: fog alpha
	float2 TextureUV1 : TEXCOORD1;
	float2 TextureUV2 : TEXCOORD2;
};

VS_OUTPUT main( VS_INPUT i )
{
    VS_OUTPUT Output;
  
	float3      Pos = 0.0f;  
	float3	Tex = float3(i.Tex0.xy, 1.0f);
	
	int nNumBones = (int)g_cbuffer.Params[0];
	int bAnimTexture = (int)g_cbuffer.Params[1];
	int bEnableFog = (int)g_cbuffer.Params[2];
	
	if(nNumBones > 0)
	{
		// Compensate for lack of UBYTE4 on Geforce3
		float 	BlendWeightsArray[4] = (float[4])i.BlendWeights;
		int IndexArray[4] = (int[4])D3DCOLORtoUBYTE4(i.BlendIndices);
		    
		float3x4 mBlendMatrix;
		mBlendMatrix = g_cbuffer.mBoneMatrixArray[IndexArray[0]] * BlendWeightsArray[0]
					 + g_cbuffer.mBoneMatrixArray[IndexArray[1]] * BlendWeightsArray[1]
					 + g_cbuffer.mBoneMatrixArray[IndexArray[2]] * BlendWeightsArray[2]	
					 + g_cbuffer.mBoneMatrixArray[IndexArray[3]] * BlendWeightsArray[3];	
		
		Pos = mul(mBlendMatrix, i.Pos);
	}
	else
	{
		Pos = (float3)i.Pos;
	}
	
	Output.Position = mul(g_cbuffer.mWorldViewProjection, float4(Pos.xyz, 1.0f));
	Output.Diffuse = g_cbuffer.vColor;
	float3 cameraPos = (float3)mul(g_cbuffer.mWorldView, float4(i.Pos.xyz, 1.0f));
	
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
			
	Output.TextureUV1.xy = i.Tex1.xy; 
	Output.TextureUV2.xy = i.Tex0.xy; 
	
    return Output;    
}

