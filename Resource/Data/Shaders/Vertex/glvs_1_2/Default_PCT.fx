#version 120

struct VSConstBuffer
{
	mat4	mWorldViewProjection;
	vec4	Params;				//0: numBones, 1: animTexture
	mat4	mTexture;
};

uniform VSConstBuffer g_vsbuffer;

void main(void)
{
    // normal MVP transform
    gl_Position = g_vsbuffer.mWorldViewProjection * gl_Vertex;

    // Copy the primary color
    gl_FrontColor = gl_Color.zyxw;

    vec3 Tex = vec3(gl_MultiTexCoord0.xy, 1.0);

    if (g_vsbuffer.Params[1] != 0.0)
	{
		Tex = vec3(g_vsbuffer.mTexture * vec4(Tex, 1.0));		 
	}
	gl_TexCoord[0].xy = Tex.xy;
}
