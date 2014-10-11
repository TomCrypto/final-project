#version 120

varying vec2 uv;
varying float light;

void main()
{
    uv = gl_MultiTexCoord0.xy;
    gl_Position = gl_Vertex;
    light = gl_Vertex.z;
}
