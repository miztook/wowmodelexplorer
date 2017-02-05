#version 150

//struct VSConstBuffer
//{
//	mat4	mWorldViewProjection;
//	mat4	mWorldView;
//	vec4	vColor;
//	vec4 	FogParams;		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
//	vec4	Params;			//0: numBones, 1: animTexture, 2: fogEnable
//	mat4	mTexture;
//};

// Matrix Pallette
//const int MAX_MATRICES = 58;
//uniform vec4 g_mBoneMatrixArray[MAX_MATRICES * 3];	

const int MAX_MATRICES = 58;

const int mWorldViewProjection = 0;
const int mWorldView = 4;
const int vColor = 8;
const int FogParams = 9;
const int Params = 10;
const int mTexture = 11;
const int mBoneMatrixArray = 15;

const int VSBUFFER_SIZE = 15 + MAX_MATRICES * 3;

uniform vec4 g_vsbuffer[VSBUFFER_SIZE];

in vec3	Pos;
in mediump vec3	Normal;
in mediump vec2	Tex0;
in mediump vec2 Tex1;
in mediump vec4	BlendWeight;
in mediump vec4 BlendIndices;

out mediump vec4 v_Diffuse;
out mediump vec3 v_Tex0;		// vertex texture coords, z: fog alpha
out mediump vec2 v_Tex1;	
out mediump vec2 v_Tex2; 	

vec4 Mul4( vec4 vInputPos, int nMatrix )
{
	vec4 vResult;
    vResult.x = dot( vInputPos, g_vsbuffer[nMatrix+0].xyzw );
    vResult.y = dot( vInputPos, g_vsbuffer[nMatrix+1].xyzw );
    vResult.z = dot( vInputPos, g_vsbuffer[nMatrix+2].xyzw );
	vResult.w = dot( vInputPos, g_vsbuffer[nMatrix+3].xyzw );
	return vResult;
}

mediump float CalcFogFactor( mediump float d, mediump vec4 fogParams )
{
	mediump float fogStart = fogParams[1];
	mediump float fogEnd = fogParams[2];
	mediump float fogDensity = fogParams[3];
	
	mediump float fogCoeff = clamp((d - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
	fogCoeff = pow(fogCoeff, fogDensity);

	return fogCoeff;
}

void main(void)
{
    vec3 pos;
	if (g_vsbuffer[Params][0] > 0.0)
	{
		mediump vec4 mBlendMatrix[3];
		lowp vec4 mBlendWeight = BlendWeight;
		int nIndex0 = int(BlendIndices.x) * 3 + mBoneMatrixArray;
		int nIndex1 = int(BlendIndices.y) * 3 + mBoneMatrixArray;
		int nIndex2 = int(BlendIndices.z) * 3 + mBoneMatrixArray;
		int nIndex3 = int(BlendIndices.w) * 3 + mBoneMatrixArray;
		
		mBlendMatrix[0] = g_vsbuffer[nIndex0] * mBlendWeight.x
						+ g_vsbuffer[nIndex1] * mBlendWeight.y
						+ g_vsbuffer[nIndex2] * mBlendWeight.z
						+ g_vsbuffer[nIndex3] * mBlendWeight.w;
						  
		mBlendMatrix[1] = g_vsbuffer[nIndex0 + 1] * mBlendWeight.x
						+ g_vsbuffer[nIndex1 + 1] * mBlendWeight.y
						+ g_vsbuffer[nIndex2 + 1] * mBlendWeight.z
						+ g_vsbuffer[nIndex3 + 1] * mBlendWeight.w;
						  
		mBlendMatrix[2] = g_vsbuffer[nIndex0 + 2] * mBlendWeight.x
						+ g_vsbuffer[nIndex1 + 2] * mBlendWeight.y
						+ g_vsbuffer[nIndex2 + 2] * mBlendWeight.z
						+ g_vsbuffer[nIndex3 + 2] * mBlendWeight.w;

		pos.x = dot( vec4(Pos, 1.0), mBlendMatrix[0].xyzw );
		pos.y = dot( vec4(Pos, 1.0), mBlendMatrix[1].xyzw );
		pos.z = dot( vec4(Pos, 1.0), mBlendMatrix[2].xyzw );
	}
	else
	{
		pos = Pos;
	}
	
    gl_Position = Mul4(vec4(pos.xyz, 1.0), mWorldViewProjection);

    // Copy the primary color
    v_Diffuse = g_vsbuffer[vColor];
	
	vec3 tex = vec3(Tex0, 1.0);
    if (g_vsbuffer[Params][1] != 0.0)
	{
		tex = vec3(Mul4(vec4(tex, 1.0), mTexture));	 
	}
	
	if (g_vsbuffer[Params][2] != 0.0)
	{
		vec3 cameraPos = vec3(Mul4(vec4(pos, 1.0), mWorldView));
		tex.z = CalcFogFactor(cameraPos.z, g_vsbuffer[FogParams]);
	}
	else
	{
		tex.z = 0.0;
	}
	
	v_Tex0.xyz = tex.xyz;
	v_Tex1.xy = Tex1.xy;	
	v_Tex2.xy = tex.xy;
}