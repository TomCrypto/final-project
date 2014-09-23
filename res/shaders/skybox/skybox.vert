#version 120

uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * gl_Vertex;
}
