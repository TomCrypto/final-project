#version 120

uniform mat4 proj;
uniform mat4 view;
uniform vec3 cam_pos;
uniform vec3 sun_pos;

void main()
{
    gl_Position = proj * view * vec4(gl_Vertex.xyz + sun_pos, 1.0);
}
