#version 150

//struct PSConstBuffer
//{
//	mediump vec4	AmbientColor;
//	mediump vec4	FogColor;
//	mediump vec4 	Params;		//0: alphatest, 1: ref	
//};

//uniform PSConstBuffer g_psbuffer;

const int AmbientColor = 0;
const int FogColor = 1;
const int Params = 2;

const int PSBUFFER_SIZE = 3;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;

in mediump vec4 v_Diffuse;
in mediump vec3 v_Tex0;

out mediump vec4 COLOR0;

void main(void)
{
	mediump vec4 col;
	col = texture2D(g_TexSampler0, v_Tex0.xy);
	
	col.rgb = col.rgb * v_Diffuse.rgb * 1.5;
	
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