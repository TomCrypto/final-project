#version 120

uniform sampler2D occlusion;
uniform sampler2D spectrum;

uniform int max_lights;
uniform float f_number;
uniform float intensity;
uniform float ghost_brightness;

varying vec2 uv;
varying float light_f;
varying float ghost_f;

vec3 get_occlusion(int light_id)
{
    float coord = float(light_id) / float(max_lights);
	return texture2D(occlusion, vec2(coord, 0.0)).rgb;
}

float luminance(vec3 rgb)
{
    return dot(rgb, vec3(0.2126, 0.7152, 0.0722));
}

int rnd;

void srand(int seed)
{
    rnd = seed;
}

float rand()
{
    rnd = int(mod(1140671485 * rnd + 12820163, 16777216));
    return float(rnd) / 16777216.0;
}

void main()
{
    float dist = length(uv * 2 - 1);

    if (dist > 1) {
        gl_FragColor = vec4(0, 0, 0, 1);
        return;
    }

    int lid = int(light_f);
    int gid = int(ghost_f);
    vec3 color = vec3(1);
    srand(gid);

    float ghost_blur = rand() * 0.45 + 0.25;
    vec3 ghost_color = (0.00005 + rand() * 0.0002)
                     * texture2D(spectrum, vec2(rand(), 0)).rgb;
    float ghost_strength = 0.1 + rand() * 0.9;


    if (dist > ghost_blur) {
        color *= pow((1 - dist) / (1 - ghost_blur), 5);
    }

    color *= luminance(get_occlusion(lid));
    color *= pow(f_number, -1);
    color *= ghost_brightness;
    color *= ghost_strength;
    color *= ghost_color;
    color *= intensity;

    gl_FragColor = vec4(color, 1);
}
