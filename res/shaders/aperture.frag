#version 120

uniform sampler2D flare;
uniform sampler2D occlusion;

uniform float intensity;

varying vec2 uv;

int resolution = 8;
int max_lights = 8; // hardcoded!

vec3 compute_avg_occlusion(int light)
{
	vec3 avg = vec3(0);

	for (int t = 0; t < resolution; ++t)
		avg += texture2D(occlusion, vec2(light / float(max_lights),
									     t / float(resolution))).rgb;

	return avg;
}

void main()
{
    gl_FragColor = vec4(texture2D(flare, uv).rgb * compute_avg_occlusion(0) * intensity, 1);
}
