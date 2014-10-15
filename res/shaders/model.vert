#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 world_pos;
varying vec3 normal;
varying vec2 uv;

void main()
{
    gl_Position = proj * view * gl_Vertex;
    world_pos = gl_Vertex.xyz;
    normal = gl_Normal.xyz;
	uv = gl_MultiTexCoord0.xy;
}
