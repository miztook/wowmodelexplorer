#version 150

//struct VSConstBuffer
//{
//	mat4	mWorldViewProjection;
//	mediump vec4	vColor;
//	mediump vec4	Params;
//	mediump mat4	mTexture;
//};

const int mWorldViewProjection = 0;
const int vColor = 4;
const int Params = 5;
const int mTexture = 6;

const int VSBUFFER_SIZE = 10;

uniform vec4 g_vsbuffer[VSBUFFER_SIZE];

in vec3 Pos;
in mediump vec2 Tex0;

out mediump vec2 v_Tex0;
out mediump vec4 v_Diffuse;

vec4 Mul4( vec4 vInputPos, int nMatrix )
{
	vec4 vResult;
    vResult.x = dot( vInputPos, g_vsbuffer[nMatrix+0].xyzw );
    vResult.y = dot( vInputPos, g_vsbuffer[nMatrix+1].xyzw );
    vResult.z = dot( vInputPos, g_vsbuffer[nMatrix+2].xyzw );
	vResult.w = dot( vInputPos, g_vsbuffer[nMatrix+3].xyzw );
	return vResult;
}

void main(void)
{
    // normal MVP transform
    gl_Position = Mul4(vec4(Pos.xyz, 1.0), mWorldViewProjection);

    // Copy the primary color
    v_Diffuse = g_vsbuffer[vColor];
	
	mediump vec3 tex = vec3(Tex0, 1.0);
    if (g_vsbuffer[Params][1]!= 0.0)
	{
		tex = vec3(Mul4(vec4(tex, 1.0), mTexture));	 
	}
	v_Tex0.xy = tex.xy;
}
