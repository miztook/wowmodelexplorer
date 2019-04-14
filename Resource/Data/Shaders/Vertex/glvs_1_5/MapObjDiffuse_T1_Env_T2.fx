#version 150

//struct VSConstBuffer
//{
//	mat4	mWorldViewProjection;
//	mat4	mWorldView;
//  mat4	mWorld;
//	vec4 	FogParams;		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
//	vec4	ClipPlane0;		
//	vec4	Params;			//0: animTexture, 1: fogEnable
//	mat4	mTexture;
//};

//uniform VSConstBuffer g_vsbuffer;

const int mWorldViewProjection = 0;
const int mWorldView = 4;
const int mWorld = 8;
const int FogParams = 12;
const int ClipPlane0 = 13;
const int Params = 14;
const int mTexture = 15;

const int VSBUFFER_SIZE = 19;

uniform vec4 g_vsbuffer[VSBUFFER_SIZE];

in vec3	Pos;
in mediump vec3	Normal;
in mediump vec4 Col0;
in mediump vec2	Tex0;

out mediump vec4 v_Diffuse;
out mediump vec3 v_Normal;
out mediump vec3 v_Tex0;		// vertex texture coords, z: fog alpha
out mediump vec2 v_Tex1;

vec3 Mul3( vec3 vInputPos, int nMatrix )
{
	vec3 vResult;
    vResult.x = dot( vInputPos, g_vsbuffer[nMatrix+0].xyz );
    vResult.y = dot( vInputPos, g_vsbuffer[nMatrix+1].xyz );
    vResult.z = dot( vInputPos, g_vsbuffer[nMatrix+2].xyz );
	return vResult;
}

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
	gl_Position = Mul4(vec4(Pos, 1.0), mWorldViewProjection);

	gl_ClipDistance[0] = dot(gl_Position, g_vsbuffer[ClipPlane0]);
		
	v_Normal = normalize(Mul3(Normal, mWorld));
	
	v_Diffuse = Col0;
	
	vec3 tex = vec3(Tex0, 1.0);
    if (g_vsbuffer[Params][0] != 0.0)
	{
		tex = vec3(Mul4(vec4(tex, 1.0), mTexture));	 
	}
	
	vec3 cameraPos = vec3(Mul4(vec4(Pos, 1.0), mWorldView));
	if (g_vsbuffer[Params][1] != 0.0)
	{
		tex.z = CalcFogFactor(cameraPos.z, g_vsbuffer[FogParams]);
	}
	else
	{
		tex.z = 0.0;
	}
	
	v_Tex0 = tex;
	
	mediump vec3 normal = vec3(Mul4(vec4(Normal, 1.0), mWorldView));
	normal = normalize(normal);
	vec3 finalPos = cameraPos.xyz - dot(cameraPos, normal) * 2.0 * normal.xyz;
	finalPos = normalize(finalPos);
	
	v_Tex1.xy = finalPos.xy * 0.5 + 0.5;
}




