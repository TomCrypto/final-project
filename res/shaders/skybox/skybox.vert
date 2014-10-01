#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 pos;

void main()
{
    pos = (view * gl_Vertex).xyz;
    gl_Position = proj * view * gl_Vertex;
}
