//struct VSConstBuffer
//{
//	mat4	mWorldViewProjection;
//	mat4	mWorldView;
//	vec4	vColor;
//	vec4 	FogParams;		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
//	vec4	Params;			//0: numBones, 1: animTexture, 2: fogEnable
//	mat4	mTexture;
//};

const int mWorldViewProjection = 0;
const int mWorldView = 4;
const int vColor = 8;
const int FogParams = 9;
const int Params = 10;
const int mTexture = 11;

const int VSBUFFER_SIZE = 15;

uniform vec4 g_vsbuffer[VSBUFFER_SIZE];

attribute vec3 Pos;
attribute mediump vec4 Col0;
attribute mediump vec2 Tex0;

varying mediump vec3 v_Tex0;
varying mediump vec4 v_Diffuse;

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
    gl_Position = Mul4(vec4(Pos.xyz, 1.0), mWorldViewProjection);

    v_Diffuse = g_vsbuffer[vColor] * Col0.zyxw;
	
	vec3 tex = vec3(Tex0, 1.0);
    if (g_vsbuffer[Params][1] != 0.0)
	{
		tex = vec3(Mul4(vec4(tex, 1.0), mTexture));	 
	}
	
	if (g_vsbuffer[Params][2] != 0.0)
	{
		vec3 cameraPos = vec3(Mul4(vec4(Pos, 1.0), mWorldView));
		tex.z = CalcFogFactor(cameraPos.z, g_vsbuffer[FogParams]);
	}
	else
	{
		tex.z = 0.0;
	}
	
	v_Tex0 = tex;
}
