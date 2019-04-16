#version 150

//struct PSConstBuffer
//{
//	mediump vec4	LightDir;		// light dir in world space
//	mediump vec4 	LightColor;		// light color;
//	mediump vec4	Diffuse;		
//  mediump vec4	Emissive;
//  mediump vec4	Specular;		//3: gloss(shiness)
//	mediump vec4	FogColor;
//	mediump vec4 	Params;		//0: alphatest, 1: ref	
//};

//uniform PSConstBuffer g_psbuffer;

const int LightDir = 0;
const int LightColor = 1;
const int Diffuse = 2;
const int Emissive = 3;
const int Specular = 4;
const int FogColor = 5;
const int Params = 6;

const int PSBUFFER_SIZE = 7;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;

in mediump vec4 v_Diffuse;
in mediump vec3 v_Normal;
in mediump vec3 v_Tex0;

out mediump vec4 COLOR0;

mediump float HalfLambert( mediump vec3 worldNormal, mediump vec3 lightDir )
{
	return clamp(dot(worldNormal, lightDir), 0.0, 1.0) * 0.5 + 0.5;
}

void main(void)
{
	mediump vec3 lightDir = -vec3(g_psbuffer[LightDir]);
	mediump vec3 lightColor = vec3(g_psbuffer[LightColor]);
	mediump vec3 diffuse = vec3(g_psbuffer[Diffuse]);
	mediump vec3 emissive = vec3(g_psbuffer[Emissive]);
	
	mediump vec4 albedo = texture2D(g_TexSampler0, v_Tex0.xy); 
	mediump vec4 col = albedo; 
	col.rgb = albedo.rgb * lightColor * (diffuse * HalfLambert(v_Normal, lightDir) + emissive);
	
#ifdef _ALPHATEST_
	if(g_psbuffer[Params][0] != 0.0 && col.a < g_psbuffer[Params][1])
		discard;
#endif
		
	//fog
	mediump float fogAlpha = v_Tex0.z;
	col.rgb = col.rgb * (1.0 - fogAlpha) + g_psbuffer[FogColor].rgb * fogAlpha;

	// Copy the primary color
    COLOR0 = col;
}