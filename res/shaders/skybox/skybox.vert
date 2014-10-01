#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 color;

void main()
{
    gl_Position = proj * view * gl_Vertex;

	vec4 viewAngle = vec4(0);
	viewAngle.x = dot(gl_LightSource[0].position.xyz,gl_Normal);
	viewAngle.y = (viewAngle.x*viewAngle.x)/2 + 2;

	vec3 topColor = vec3(0, 0, 1);
	vec3 bottomColor = vec3(1, 1, 1);

	color = gl_MultiTexCoord0.xyz;
}
