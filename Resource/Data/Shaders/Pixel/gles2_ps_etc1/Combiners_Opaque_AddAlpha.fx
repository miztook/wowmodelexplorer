
//#define _ALPHATEST_
//struct PSConstBuffer
//{
//	mediump vec4	FogColor;
//	mediump vec4	TextureParams;		//etc flag	
//	mediump vec4 	Params;		//0: alphatest, 1: ref
//};

//uniform PSConstBuffer g_psbuffer;

const int FogColor = 0;
const int TextureParams = 1;
const int Params = 2;

const int PSBUFFER_SIZE = 3;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;
uniform sampler2D	g_TexSampler1;

varying mediump vec4 v_Diffuse;
varying mediump vec3 v_Tex0;
varying mediump vec2 v_Tex1;

//
#define USE_ETC1_REPERAT_ADDRESS	2.0
#define USE_ETC1	1.0

mediump vec4 SampleTexture2D(sampler2D texSampler, mediump vec2 coordUV, int index)
{
	mediump vec4 col;
	if(g_psbuffer[TextureParams][index] == USE_ETC1_REPERAT_ADDRESS)			//etc1, repeat coord
	{
		mediump float v = (coordUV.y - floor(coordUV.y)) * 0.5;
		col.rgb = texture2D(texSampler, vec2(coordUV.x, v)).rgb;
		col.a = texture2D(texSampler, vec2(coordUV.x, v + 0.5)).r;
	}
	else if(g_psbuffer[TextureParams][index] == USE_ETC1)		//etc1, non repeat coord
	{
		mediump float v = coordUV.y * 0.5;
		col.rgb = texture2D(texSampler, vec2(coordUV.x, v)).rgb;
		col.a = texture2D(texSampler, vec2(coordUV.x, v + 0.5)).r;
	}
	else					//non etc1
	{
		col = texture2D(texSampler, coordUV);
	}
	return col;
}

void main(void)
{
	mediump vec4 col;
	mediump vec4 tex0 = SampleTexture2D(g_TexSampler0, v_Tex0.xy, 0);
	mediump vec4 tex1 = SampleTexture2D(g_TexSampler1, v_Tex1.xy, 1);
	
	col.rgb	= tex0.rgb * (1.0 + tex1.a);
	col.rgb = col.rgb * v_Diffuse.rgb;
	col.a = v_Diffuse.a;
	
#ifdef _ALPHATEST_
	if(g_psbuffer[Params][0] != 0.0 && col.a < g_psbuffer[Params][1])
		discard;
#endif
		
	//fog
	mediump float fogAlpha = v_Tex0.z;
	col.rgb = col.rgb * (1.0 - fogAlpha) + g_psbuffer[FogColor].rgb * fogAlpha;

	// Copy the primary color
    gl_FragColor = col;
}