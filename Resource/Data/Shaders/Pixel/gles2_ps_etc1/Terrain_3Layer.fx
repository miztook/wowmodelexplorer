//struct PSConstBuffer
//{
//	mediump vec4 	Params;		//0: numTextures
//	mediump vec4	FogColor;	
//  mediump vec4	TextureParams;
//};

//uniform PSConstBuffer g_psbuffer;

const int Params = 0;
const int FogColor = 1;
const int TextureParams = 2;

const int PSBUFFER_SIZE = 3;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;
uniform sampler2D	g_TexSampler1;
uniform sampler2D	g_TexSampler2;
uniform sampler2D	g_TexSampler3;

varying mediump vec4 v_Diffuse;
varying mediump vec4 v_Specular;
varying mediump vec2 v_Tex0;
varying mediump vec3 v_Tex1;

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
	mediump float fogAlpha = v_Tex1.z;
	
	mediump vec4 alpha = texture2D(g_TexSampler0, v_Tex1.xy);	
	mediump vec4 color = SampleTexture2D(g_TexSampler1, v_Tex0.xy, 0);
	mediump vec4 specular = v_Specular;
	
	//layer2
	mediump vec4 color1 = SampleTexture2D(g_TexSampler2, v_Tex0.xy, 1); 
	color = color1 * alpha.r + color * (1.0 - alpha.r);
	
	//layer3
	mediump vec4 color2 = SampleTexture2D(g_TexSampler3, v_Tex0.xy, 2); 
	color = color2 * alpha.g + color * (1.0 - alpha.g);
	
	//blend
	mediump float x = (1.0 - alpha.a) * 0.3 + 0.69999999;	
	color.rgb = color.rgb * x * v_Diffuse.rgb + (1.0 - alpha.a) * color.a * specular.rgb;	
	
	//fog
	color.rgb = color.rgb * (1.0 - fogAlpha) + g_psbuffer[FogColor].rgb * fogAlpha;

	color.a = 1.0;
	
	gl_FragColor = color;
}