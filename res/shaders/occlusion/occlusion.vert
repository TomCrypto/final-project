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

uniform sampler2D render;

varying vec3 total_occlusion;

int compute_lod(vec4 light_pos, float light_radius)
{
    // work out distance from light to camera
    vec3 lpos = light_pos.xyz - view_pos * light_pos.w;
    float dist = length(lpos);

    // then lod is given by C * radius / distance
    // where C is an arbitrary quality constant
    // (here we set C = 400)

    float lod = 400 * light_radius / dist;

    // clamp lod between 1 and 64 for performance

    return int(clamp(lod, 1.0, 64.0));
}

void main()
{
	gl_Position = gl_Vertex;

	int lid = int((gl_Vertex.x + 1.0) / 2.0 * max_lights);

    int lod = compute_lod(lights[lid].pos, lights[lid].radius);

    int x_res = lod;
    int y_res = lod;

    vec3 total = vec3(0.0);

    for (int y = 0; y < y_res; ++y) {
        float theta = y / float(y_res) * 3.14159265;

        for (int x = 0; x < x_res; ++x) {
            float phi = x / float(x_res) * 3.14159265 * 2.0;

            vec4 pos = lights[lid].pos + vec4(sin(theta) * cos(phi),
                                              cos(theta),
                                              sin(theta) * sin(phi),
                                              0.0) * lights[lid].radius;

            vec4 projected = viewproj * pos;
            projected.xy /= projected.w;

            vec2 sample_pos = (projected.xy + 1.0) / 2.0;

            if ((sample_pos.x >= 0) && (sample_pos.y >= 0)
		     && (sample_pos.x <= 1) && (sample_pos.y <= 1)) {
			    total += texture2D(render, sample_pos).rgb;
		    }
        }
    }

	total_occlusion = total / (x_res * y_res * 2.0);
}
