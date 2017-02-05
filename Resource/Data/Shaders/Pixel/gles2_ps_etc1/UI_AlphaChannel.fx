uniform sampler2D	g_TexSampler0;

varying mediump vec4 v_Diffuse;
varying mediump vec2 v_Tex0;

void main(void)
{
	mediump vec4 col;
	mediump vec4 texCol = texture2D(g_TexSampler0, v_Tex0.xy);
	col.rgb = texCol.rgb * v_Diffuse.rgb;

	col.a = texCol.a;

    // Copy the primary color
    gl_FragColor = col;
}