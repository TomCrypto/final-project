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
	if (shininess > 0)
        color = color + ks*pow(max(0,dot(reflect(S,N),V)),shininess);

    // fog here
    
    vec3 horizon_color = vec3(0, 0.15, 0.85);
    
    float fog_falloff = 40.0; // lose 50% intensity each X units travelled
    float dist = length(world_pos - camera_pos);
    
    if (dist > fog_falloff) {
        color = mix(color, horizon_color, 1.0 - pow(2, - (dist - fog_falloff) / fog_falloff));
    }

    gl_FragColor = vec4(color,1);
}
