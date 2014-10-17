#version 120

// light_f will contain the index of the light currently being rendered with
// lens flare

varying vec2 uv;
varying float light_f;

void main()
{
	light_f = gl_MultiTexCoord0.z;
    uv = gl_MultiTexCoord0.xy;
    gl_Position = gl_Vertex;
}
