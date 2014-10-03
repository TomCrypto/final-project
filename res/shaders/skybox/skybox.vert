#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 norm;

void main()
{
norm = gl_Normal;
gl_Position = proj * view * gl_Vertex;
}