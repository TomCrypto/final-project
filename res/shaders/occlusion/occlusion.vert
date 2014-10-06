#version 120

struct light
{
    vec4 pos;
    //vec3 strength;
	float radius;
};

//uniform int resolution;
uniform int max_lights;

uniform mat4 viewproj;
//uniform vec3 view_dir;
//uniform vec3 view_pos;

uniform light lights[8];
uniform int light_count;

uniform sampler2D render;

varying vec3 total_occlusion;

void main()
{
	gl_Position = gl_Vertex;

    /* The light that is being sampled now. */
	int light = int(gl_Normal.x * max_lights);

	/* Radial sample band. */
	float band = gl_Normal.y;

	// work out the apparent size of the light from the camera
	vec4 center_clip_space = viewproj * lights[light].pos;
	vec4 edgepx_clip_space = viewproj * (lights[light].pos + vec4(+1, 0, 0, 0) * lights[light].radius);
	vec4 edgenx_clip_space = viewproj * (lights[light].pos + vec4(-1, 0, 0, 0) * lights[light].radius);
	vec4 edgepy_clip_space = viewproj * (lights[light].pos + vec4(0, +1, 0, 0) * lights[light].radius);
	vec4 edgeny_clip_space = viewproj * (lights[light].pos + vec4(0, -1, 0, 0) * lights[light].radius);
	vec4 edgepz_clip_space = viewproj * (lights[light].pos + vec4(0, 0, +1, 0) * lights[light].radius);
	vec4 edgenz_clip_space = viewproj * (lights[light].pos + vec4(0, 0, -1, 0) * lights[light].radius);
	vec2 center = center_clip_space.xy / center_clip_space.w;
	vec2 edgepx = edgepx_clip_space.xy / edgepx_clip_space.w;
	vec2 edgenx = edgenx_clip_space.xy / edgenx_clip_space.w;
	vec2 edgepy = edgepy_clip_space.xy / edgepy_clip_space.w;
	vec2 edgeny = edgeny_clip_space.xy / edgeny_clip_space.w;
	vec2 edgepz = edgepz_clip_space.xy / edgepz_clip_space.w;
	vec2 edgenz = edgenz_clip_space.xy / edgenz_clip_space.w;

	float dist = 0;
	dist = max(dist, length(center - edgepx));
	dist = max(dist, length(center - edgenx));
	dist = max(dist, length(center - edgepy));
	dist = max(dist, length(center - edgeny));
	dist = max(dist, length(center - edgepz));
	dist = max(dist, length(center - edgenz));

	vec3 total = vec3(0.0);

	// finally, sample the radial band accordingly
	const int samples = 16;

	for (int t = 0; t < samples; ++t) {
		vec2 sample_pos = vec2(cos(2 * 3.14159265 * t / float(samples)),
							   sin(2 * 3.14159265 * t / float(samples))) * dist * band
						+ center;

		sample_pos = sample_pos * 0.5 + vec2(0.5);

		if ((sample_pos.x >= 0) && (sample_pos.y >= 0)
		 && (sample_pos.x <= 1) && (sample_pos.y <= 1)) {
			total += texture2D(render, sample_pos).rgb;
		}
	}

	total_occlusion = total / samples;
}
