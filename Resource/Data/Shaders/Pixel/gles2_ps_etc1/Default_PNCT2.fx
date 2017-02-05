//struct PSConstBuffer
//{
//	vec4 	TextureParams;
//	vec4	Params;		//0: alphatest, 1: ref	
//};

//uniform PSConstBuffer g_psbuffer;

const int TextureParams = 0;
const int Params = 1;

const int PSBUFFER_SIZE = 2;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;

varying mediump vec2 v_Tex0;
varying mediump vec4 v_Diffuse;

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
	mediump vec4 color = SampleTexture2D(g_TexSampler0, v_Tex0.xy, 0) * v_Diffuse;
	
#ifdef _ALPHATEST_
	if(g_psbuffer[Params][0] != 0.0 && color.a < g_psbuffer[Params][1])
		discard;
#endif
		
    // Copy the primary color
    gl_FragColor = color;
}