#version 120

uniform int max_lights;

uniform sampler2D flare;
uniform sampler2D occlusion;

uniform float intensity;
uniform float f_number;

varying vec2 uv;
varying float light_f;

vec3 get_occlusion(int light_id)
{
    float coord = float(light_id) / float(max_lights);
    return texture2D(occlusion, vec2(coord, 0.0)).rgb;
}

void main()
{
	int lid = int(light_f);

    float noise_threshold = 1e-7;

	gl_FragColor = vec4(max(vec3(0.0), texture2D(flare, uv).rgb - noise_threshold) * get_occlusion(lid) * intensity * pow(f_number, -1.0), 1);
}
