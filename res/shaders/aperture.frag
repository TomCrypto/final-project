#version 120

struct light
{
    vec4 pos;
	float radius;
};

uniform mat4 viewproj;
uniform vec3 view_pos;

uniform int max_lights;
uniform light lights[8];

uniform sampler2D flare;
uniform sampler2D occlusion;

uniform float intensity;

varying vec2 uv;
varying float light_f;

vec3 get_occlusion(int light)
{
	return texture2D(occlusion, vec2(light / float(max_lights), 0)).rgb;
}

void main()
{
	int light = int(light_f);

	gl_FragColor = vec4(texture2D(flare, uv).rgb * get_occlusion(light) * intensity, 1);
}
