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
uniform lightSource[16] lights;
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
		if(lights[i].pos.w == 0.0f) {
			att = 1.0f;
			lightDir = normalize(lights[i].pos.xyz);
		}
		else {
			vec3 posToLight = lights[i].pos.xyz-world_pos;
			float distance = length(posToLight);
			lightDir = normalize(posToLight);
			att = 1.0f /(dot(lights[i].attenuation,vec3(1,distance,distance*distance)));
		}
		vec3 intensity = att*lights[i].intensity;
		vec3 diffuse = intensity*newKd*max(0.0,dot(N,lightDir));
		vec3 spec = vec3(0);
		if(dot(N,lightDir)>=0.0f && shininess>0) {
			vec3 V = normalize(world_pos - camera_pos);
			spec = intensity*ks*pow(max(0,dot(reflect(-lightDir,N),V)),shininess);
		}
		color = color + diffuse + spec;
	}

    gl_FragColor = vec4(color*0.001,alpha);

	// fog here
    //vec3 horizon_color = vec3(0, 0.15, 0.85);
    //float fog_falloff = 40.0; // lose 50% intensity each X units travelled
    //float dist = length(world_pos - camera_pos);
	//if (dist > fog_falloff) {
        //color = mix(color, horizon_color, 1.0 - pow(2, - (dist - fog_falloff) / fog_falloff));
    //}
}
