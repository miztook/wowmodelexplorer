#version 120

struct VSConstBuffer
{
	mat4	mWorldViewProjection;
	vec4	vColor;
};

uniform VSConstBuffer g_vsbuffer;

void main(void)
{
    // normal MVP transform
    gl_Position = g_vsbuffer.mWorldViewProjection * gl_Vertex;

    // Copy the primary color
    gl_FrontColor = g_vsbuffer.vColor;
}
