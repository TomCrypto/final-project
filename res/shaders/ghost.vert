#version 120

varying vec2 uv;
varying float light_f;

void main()
{
    light_f = gl_MultiTexCoord0.z;
    uv = gl_MultiTexCoord0.xy;
    gl_Position = gl_Vertex;
}
