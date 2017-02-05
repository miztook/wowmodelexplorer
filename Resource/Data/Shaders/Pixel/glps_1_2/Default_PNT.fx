#version 120

uniform sampler2D	g_TexSampler0;

void main(void)
{
	gl_FragColor = texture2D(g_TexSampler0, gl_TexCoord[0].xy) * gl_Color;
}