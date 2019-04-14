#version 150

//struct PSConstBuffer
//{
//	mediump vec4	LightDir;		// light dir in world space
//	mediump vec4 	LightColor;		// light color;
//	mediump vec4	Diffuse;		
//  mediump vec4	Emissive;
//	mediump vec4	FogColor;
//	mediump vec4 	Params;		//0: alphatest, 1: ref	
//};

//uniform PSConstBuffer g_psbuffer;

const int LightDir = 0;
const int LightColor = 1;
const int Diffuse = 2;
const int Emissive = 3;
const int FogColor = 4;
const int Params = 5;

const int PSBUFFER_SIZE = 6;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;
uniform sampler2D	g_TexSampler1;

in mediump vec4 v_Diffuse;
in mediump vec3 v_Normal;
in mediump vec3 v_Tex0;
in mediump vec2 v_Tex1;

out mediump vec4 COLOR0;

mediump float HalfLambert( mediump vec3 worldNormal, mediump vec3 lightDir )
{
	return clamp(dot(worldNormal, -lightDir), 0.0, 1.0) * 0.5 + 0.5;
}

void main(void)
{
	mediump vec3 lightDir = vec3(g_psbuffer[LightDir]);
	mediump vec3 lightColor = vec3(g_psbuffer[LightColor]);
	mediump vec3 diffuse = vec3(g_psbuffer[Diffuse]);
	mediump vec3 emissive = vec3(g_psbuffer[Emissive]);
	
	mediump vec4 col;
	mediump vec4 tex0 = texture2D(g_TexSampler0, v_Tex0.xy);
	mediump vec4 tex1 = texture2D(g_TexSampler1, v_Tex1.xy);

	col.rgb = tex0.rgb * tex0.a + tex1.rgb * (1.0 - tex0.a);
	col.rgb = col.rgb * (lightColor.rgb * diffuse * HalfLambert(v_Normal, lightDir) + emissive);
	col.a = 1.0;
		
	//fog
	mediump float fogAlpha = v_Tex0.z;
	col.rgb = col.rgb * (1.0 - fogAlpha) + g_psbuffer[FogColor].rgb * fogAlpha;

	// Copy the primary color
    COLOR0 = col;
}