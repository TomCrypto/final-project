#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 color;

void main()
{
    vec3 pos = gl_Vertex.xyz;
    gl_Position = proj * view * gl_Vertex;

	float phi = atan(pos.x, pos.z);
	float theta = acos(pos.y / length(pos));

	vec3 topColor = vec3(0, 0, 1);
	vec3 bottomColor = vec3(1, 1, 1);

	color = mix(topColor, bottomColor, pow(theta / 3.14159265 * 1.9, 5));
}
