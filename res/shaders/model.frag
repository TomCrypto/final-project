#version 120

struct light
{
    vec3 pos;
    //vec3 intensity;
    //float falloff;
};

uniform int light_count;
uniform light lights[8];
uniform vec3 camera_pos;

uniform vec3 ks, kd, ka;
uniform float shininess;

varying vec3 world_pos;
varying vec3 normal;

void main()
{
gl_FragColor = vec4(1,0,0, 1);
return;
    vec3 N = normalize(normal);

    vec3 color = ka;

    for (int t = 0; t < light_count; ++t) {
        vec3 sample = vec3(0);

        vec3 L = world_pos - lights[t].pos;
        float dist = length(L);
        L /= dist;
        vec3 V = normalize(world_pos - camera_pos);
        vec3 R = reflect(L, N);

        float NdL = max(0.0, dot(N, L));
        float RdV = max(0.0, dot(R, V));

        sample += kd * NdL * 10; /* lights[t].intensity */;
        sample += ks * pow(RdV, shininess) * 10; /* lights[t].intensity */;

        sample /= pow(dist, /* falloff */ 0);
        color += sample;
    }

    gl_FragColor = vec4(color, 1);
}
