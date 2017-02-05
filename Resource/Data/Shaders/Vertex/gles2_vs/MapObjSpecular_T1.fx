//struct VSConstBuffer
//{
//	mat4	mWorldViewProjection;
//	mat4	mWorldView;
//	vec4	LightDir;		// light dir in world space
//	vec4 	LightColor;		// light color;
//	vec4	MaterialParams;		//0: ambient, 1: diffuse, 2: specular
//	vec4 	FogParams;		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
//	vec4	Params;			//0: animTexture, 1: fogEnable
//	mat4	mTexture;
//};

//uniform VSConstBuffer g_vsbuffer;

const int mWorldViewProjection = 0;
const int mWorldView = 4;
const int LightDir = 8;
const int LightColor = 9;
const int MaterialParams = 10;
const int FogParams = 11;
const int Params = 12;
const int mTexture = 13;

const int VSBUFFER_SIZE = 17;

uniform vec4 g_vsbuffer[VSBUFFER_SIZE];

attribute vec3	Pos;
attribute mediump vec3	Normal;
attribute mediump vec4 Col0;
attribute mediump vec2	Tex0;

varying mediump vec4 v_Diffuse;
varying mediump vec4 v_Specular;
varying mediump vec3 v_Tex0;		// vertex texture coords, z: fog alpha

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
	mediump float ambient = g_vsbuffer[MaterialParams][0];
	mediump float diffuse = g_vsbuffer[MaterialParams][1];
	mediump float specular = g_vsbuffer[MaterialParams][2];
	
	gl_Position = Mul4(vec4(Pos, 1.0), mWorldViewProjection);

	mediump vec3 lightDir = vec3(g_vsbuffer[LightDir]);
	mediump vec3 normal = Mul3(Normal.xyz, mWorldView);
	normal = normalize(normal);
	
	v_Specular = vec4(specular, specular, specular, 1.0);
	v_Diffuse.rgb = vec3(g_vsbuffer[LightColor] * clamp(dot(normal, -lightDir), 0.0, 1.0)) * diffuse + ambient;
	v_Diffuse.a = Col0.a;
	
	vec3 tex = vec3(Tex0, 1.0);
    if (g_vsbuffer[Params][0] != 0.0)
	{
		tex = vec3(Mul4(vec4(tex, 1.0), mTexture));	 
	}
	
	if (g_vsbuffer[Params][1] != 0.0)
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




