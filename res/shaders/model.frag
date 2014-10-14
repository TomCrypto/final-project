#version 120

uniform vec4 sun_dir;
uniform vec3 camera_pos;

uniform vec3 ks, kd, ka;
uniform float shininess;

varying vec3 world_pos;
varying vec3 normal;

void main()
{
    vec3 N = normalize(normal);
	vec3 S = normalize(sun_dir.xyz);
	vec3 V = normalize(world_pos - camera_pos);

	vec3 color = max(0.1,dot(N,S))*kd;
	color = color + ks*pow(max(0,dot(reflect(-S,N),V)),shininess);
    gl_FragColor = vec4(color,1);
}
