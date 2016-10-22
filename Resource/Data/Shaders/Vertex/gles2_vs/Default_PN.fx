//struct VSConstBuffer
//{
//	mat4	mWorldViewProjection;
//	vec4	vColor;
//};

const int mWorldViewProjection = 0;
const int vColor = 4;

const int VSBUFFER_SIZE = 5;

uniform vec4 g_vsbuffer[VSBUFFER_SIZE];

attribute vec3 Pos;
attribute mediump vec3 Normal;

varying mediump vec4 v_Diffuse;

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
}
