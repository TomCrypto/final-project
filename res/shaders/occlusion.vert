#version 120

struct light
{
    vec4 pos;
	float radius;
	int partial_occlusion;
};

uniform mat4 viewproj;

uniform light lights[8];

uniform sampler2D render;

varying vec3 total_occlusion;

int compute_lod(vec4 light_pos, float light_radius)
{
    return 10;
}

void main()
{
	gl_Position = vec4(gl_Vertex.xy, 0.5, 1.0);

	int lid = int(gl_Vertex.z);

	int lod = compute_lod(lights[lid].pos, lights[lid].radius);

	if (lights[lid].partial_occlusion == 1) {
        vec3 total = vec3(0.0);

        for (int y = 0; y < lod; ++y) {
            float theta = y / float(lod) * 3.14159265;

            for (int x = 0; x < lod; ++x) {
                float phi = x / float(lod) * 3.14159265 * 2.0;

                vec4 pos = lights[lid].pos + vec4(sin(theta) * cos(phi),
                                                  cos(theta),
                                                  sin(theta) * sin(phi),
                                                  0.0) * (lights[lid].radius * 0.98);

                vec4 projected = viewproj * pos;
                projected.xy /= projected.w;

                vec2 sample_pos = (projected.xy + 1.0) / 2.0;

                if ((sample_pos.x >= 0) && (sample_pos.y >= 0)
                 && (sample_pos.x <= 1) && (sample_pos.y <= 1)) {
                    total += max(vec3(0.0), texture2D(render, sample_pos).rgb - vec3(1e1));
                }
            }
        }

        // About 50% of samples will be back-facing
        total_occlusion = total / (lod * lod * 2.0);
    } else {
        vec3 total = vec3(0.0);

        for (int y = 0; y < lod; ++y) {
            float theta = y / float(lod) * 3.14159265;

            for (int x = 0; x < lod; ++x) {
                float phi = x / float(lod) * 3.14159265 * 2.0;

                vec4 pos = lights[lid].pos + vec4(sin(theta) * cos(phi),
                                                  cos(theta),
                                                  sin(theta) * sin(phi),
                                                  0.0) * (lights[lid].radius * 0.98);

                vec4 projected = viewproj * pos;
                projected.xy /= projected.w;

                vec2 sample_pos = (projected.xy + 1.0) / 2.0;

                if ((sample_pos.x >= 0) && (sample_pos.y >= 0)
                 && (sample_pos.x <= 1) && (sample_pos.y <= 1)) {
                    total = max(total, texture2D(render, sample_pos).rgb - vec3(1e1));
                }
            }
        }

        // About 50% of samples will be back-facing
        total_occlusion = total;
    }
}
