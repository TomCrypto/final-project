#version 120
uniform vec3 camera_pos;

uniform vec3 ks, kd, ka;
uniform float shininess;

varying vec3 world_pos;
varying vec3 normal;
varying vec2 uv;

struct lightSource {
vec4 pos;
vec3 intensity;
vec3 attenuation; // Constant/linear/quadratic (xyz)
};
uniform lightSource[8] lights;
uniform int noOfLights;

uniform int textureSet;
uniform sampler2D tex;

const vec3 ambient = vec3(0.2, 0.2, 0.2);

void main()
{
    vec3 N = normalize(normal);

	vec3 newKd = kd;
	float alpha = 1;
	if(textureSet>0) {
		newKd = texture2D(tex, uv).rgb;
	}
	if(textureSet>1) {
		alpha = texture2D(tex, uv).a;
	}

	vec3 color = ambient*ka;
	float att;
	vec3 lightDir;
	for(int i=0;i<noOfLights;i++) {
        lightDir = lights[i].pos.xyz - world_pos * lights[i].pos.w;
        float distance = length(lightDir);
        lightDir /= distance;

        float att = 1.0 / dot(lights[i].attenuation, vec3(1, distance, distance*distance));

        float NdotLDir = max(0.0, dot(N, lightDir));
		vec3 intensity = att*lights[i].intensity;
		vec3 diffuse = intensity*newKd*NdotLDir;
		vec3 spec = vec3(0);

		if(shininess>0) {
			vec3 V = normalize(world_pos - camera_pos);
			spec = intensity*ks*pow(max(0,dot(reflect(-lightDir,N),V)),shininess);
		}

		color = color + diffuse + spec;
	}

    color *= 0.001;

    vec3 horizon_color = vec3(0, 0.15, 0.85);
    float fog_falloff = 30.0; // lose 50% intensity each X units travelled
    float dist = length(world_pos - camera_pos);
	if (dist > fog_falloff) {
        color = mix(color, horizon_color, 1.0 - pow(2, - (dist - fog_falloff) / fog_falloff));
    }

    gl_FragColor = vec4(color, alpha);
}
