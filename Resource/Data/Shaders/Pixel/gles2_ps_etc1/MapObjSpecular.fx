
//struct PSConstBuffer
//{
//	mediump vec4	AmbientColor;
//	mediump vec4	FogColor;
//	mediump vec4	TextureParams;
//	mediump vec4 	Params;		//0: alphatest, 1: ref	
//};

//uniform PSConstBuffer g_psbuffer;

const int AmbientColor = 0;
const int FogColor = 1;
const int TextureParams = 2;
const int Params = 3;

const int PSBUFFER_SIZE = 4;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;

varying mediump vec4 v_Diffuse;
varying mediump vec4 v_Specular;
varying mediump vec3 v_Tex0;

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
	
	col.rgb = tex0.rgb * v_Diffuse.rgb + tex0.rgb * tex0.a * v_Specular.rgb;
	col.a = tex0.a;
	
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