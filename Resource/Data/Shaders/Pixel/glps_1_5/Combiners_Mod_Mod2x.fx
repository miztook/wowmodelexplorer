#version 150

//#define _ALPHATEST_
//struct PSConstBuffer
//{
//	mediump vec4	FogColor;	
//	mediump vec4 	Params;		//0: alphatest, 1: ref
//};

//uniform PSConstBuffer g_psbuffer;

const int FogColor = 0;
const int Params = 1;

const int PSBUFFER_SIZE = 2;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;
uniform sampler2D	g_TexSampler1;

in mediump vec4 v_Diffuse;
in mediump vec3 v_Tex0;
in mediump vec2 v_Tex1;

out mediump vec4 COLOR0;

void main(void)
{
	mediump vec4 col;
	mediump vec4 tex0 = texture2D(g_TexSampler0, v_Tex0.xy);
	mediump vec4 tex1 = texture2D(g_TexSampler1, v_Tex1.xy);
	
	col = tex0 * tex1 * 2.0;
	col = v_Diffuse * col;
	
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