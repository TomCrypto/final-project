#version 120

uniform mat4 proj;
uniform mat4 view;
uniform vec3 sun_pos;

void main()
{
    gl_Position = proj * view * (gl_Vertex + vec4(sun_pos, 0.0));
}
