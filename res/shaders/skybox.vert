#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 pos;

void main()
{
gl_Position = proj * view * gl_Vertex;
pos = gl_Vertex.xyz;
}
