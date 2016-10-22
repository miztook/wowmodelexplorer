#version 150

//struct PSConstBuffer
//{
//	mediump vec4	AmbientColor;
//	mediump vec4	FogColor;
//};

//uniform PSConstBuffer g_psbuffer;

const int AmbientColor = 0;
const int FogColor = 1;

const int PSBUFFER_SIZE = 2;

uniform mediump vec4 g_psbuffer[PSBUFFER_SIZE];

uniform sampler2D	g_TexSampler0;

in mediump vec4 v_Diffuse;
in mediump vec3 v_Tex0;

out mediump vec4 COLOR0;

void main(void)
{	
	mediump vec4 col;
	mediump vec4 tex0 = texture2D(g_TexSampler0, v_Tex0.xy);
	
	col.rgb = tex0.rgb * v_Diffuse.rgb;
	col.a = 1.0;
		
	//fog
	mediump float fogAlpha = v_Tex0.z;
	col.rgb = col.rgb * (1.0 - fogAlpha) + g_psbuffer[FogColor].rgb * fogAlpha;

	// Copy the primary color
    COLOR0 = col;
}