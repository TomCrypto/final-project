#version 130

in vec3 vertexPosition;
out vec2 uv;

void main(void)
{
	gl_Position = vec4(vertexPosition.xy * 2 - 1, 0.5, 1.0);
	uv = (vec2(gl_Position.x, gl_Position.y) + vec2(1.0)) / vec2(2.0);
}