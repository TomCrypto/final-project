#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 color;

void main()
{
    vec3 pos = gl_Normal.xyz;
    gl_Position = proj * view * gl_Vertex;



	float phi = atan(pos.x, pos.z);
	float theta = acos(pos.y / length(pos));

	vec3 topColor = vec3(0, 0, 1);
	vec3 bottomColor = vec3(1, 1, 1);

	color = max(0.0, dot(-pos,gl_LightSource[0].position.xyz))*bottomColor;
}
