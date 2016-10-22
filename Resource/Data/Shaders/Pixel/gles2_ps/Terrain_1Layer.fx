
//struct PSConstBuffer
//{
//	mediump vec4 	Params;		//0: numTextures
//	mediump vec4	FogColor;	
//};

//uniform PSConstBuffer g_psbuffer;

const int Params = 0;
const int FogColor = 1;

const int PSBUFFER_SIZE = 2;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;			//blend map
uniform sampler2D	g_TexSampler1;

varying mediump vec4 v_Diffuse;
varying mediump vec4 v_Specular;
varying mediump vec2 v_Tex0;
varying mediump vec3 v_Tex1;

void main(void)
{
	mediump float fogAlpha = v_Tex1.z;
	
	mediump vec4 alpha = texture2D(g_TexSampler0, v_Tex1.xy);	
	mediump vec4 color = texture2D(g_TexSampler1, v_Tex0.xy);
	mediump vec4 specular = v_Specular;
		
	//blend
	mediump float x = (1.0 - alpha.a) * 0.3 + 0.69999999;	
	color.rgb = color.rgb * x * v_Diffuse.rgb + (1.0 - alpha.a) * color.a * specular.rgb;	
	
	//fog
	color.rgb = color.rgb * (1.0 - fogAlpha) + g_psbuffer[FogColor].rgb * fogAlpha;

	color.a = 1.0;
	
	gl_FragColor = color;
}