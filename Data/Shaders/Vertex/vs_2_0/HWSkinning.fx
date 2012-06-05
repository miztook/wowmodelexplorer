// <effectEd direct3D="9" profile="fx_2_0" shaderFlags="None, WarningsAreErrors" />

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

struct ConstantBuffer
{
	float4x4 	mWorldViewProjection;    // World * View * Projection matrix
	float4 		MaterialAmbient;
	float4		AmbientColor;
	float4		EmissiveColor;
	float4		Params;				//0: numBones, 1: animTexture
	float4x4	mTexture;
};

ConstantBuffer g_cbuffer : register(c0);

// Matrix Pallette
static const int MAX_MATRICES = 58;
float3x4    g_mBoneMatrixArray[MAX_MATRICES];			// Transposed for efficiency


struct VS_INPUT
{
    float4  Pos             : POSITION;
    float4  BlendWeights    : BLENDWEIGHT;
    float4  BlendIndices    : BLENDINDICES;
    float3  Normal          : NORMAL;
	float3  Tex0            : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
	float4  Diffuse   : COLOR0;
	float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};

VS_OUTPUT main( VS_INPUT i )
{
    VS_OUTPUT Output;
  
	float3      Pos = 0.0f;  
	float3		Tex = 0.0f;
	Tex.xy = i.Tex0.xy;
	Tex.z = 1.0f;
	
	int nNumBones = (int)g_cbuffer.Params[0];
	int bAnimTexture = (int)g_cbuffer.Params[1];
	 
	if(nNumBones > 0)
	{
		// Compensate for lack of UBYTE4 on Geforce3
		float 	BlendWeightsArray[4] = (float[4])i.BlendWeights;
		int IndexArray[4] = (int[4])D3DCOLORtoUBYTE4(i.BlendIndices);
		    
		float3x4	mBlendMatrix;   
	
		if(nNumBones == 1)
		{
			Pos = mul(g_mBoneMatrixArray[IndexArray[0]], i.Pos);
		}
		else
		{
			mBlendMatrix = g_mBoneMatrixArray[IndexArray[0]] * BlendWeightsArray[0];
			mBlendMatrix += g_mBoneMatrixArray[IndexArray[1]] * BlendWeightsArray[1];			
			mBlendMatrix += g_mBoneMatrixArray[IndexArray[2]] * BlendWeightsArray[2];	
			mBlendMatrix += g_mBoneMatrixArray[IndexArray[3]] * BlendWeightsArray[3];	
			
			Pos = mul(mBlendMatrix, i.Pos);
		}
	}
	else
	{
		Pos = (float3)i.Pos;
	}
	
	Output.Position = mul(g_cbuffer.mWorldViewProjection, float4(Pos.xyz, 1.0f));		
	Output.Diffuse = g_cbuffer.MaterialAmbient * g_cbuffer.AmbientColor + g_cbuffer.EmissiveColor; 	
	
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

