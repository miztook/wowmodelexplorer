#version 120

uniform sampler2D	g_TexSampler0;

void main(void)
{
	vec4 col;
	vec4 texCol = texture2D(g_TexSampler0, gl_TexCoord[0].xy);
	col.rgb = texCol.rgb * gl_Color.rgb;
	col.a = texCol.a;

    // Copy the primary color
    gl_FragColor = col;
}
