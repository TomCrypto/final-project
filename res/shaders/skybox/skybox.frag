#version 120

float ray_sphere(vec3 p, vec3 d, vec3 o, float r) {
    vec3 l = o - p;
    float a = dot(d, d);
    float b = 2 * dot(d, l);
    float c = dot(l, l) - r * r;

    float discr = b * b - 4 * a * c;
    if (discr < 0.0) return 0.0;
    // assert discr >= 0

    return (-b + sqrt(discr)) / (2 * a);
}

// computes the distance in meters from the position to the edge of the atmosphere
// assuming the earth is at (0, -6371km, 0) and the atmosphere has height 50km
//
// assumes the point is between the earth's surface and atmosphere
// dir need not be normalized
float atmospheric_depth(vec3 pos, vec3 dir) {
    const vec3 earth_center = vec3(0, -6371e3, 0);
    const float atmo_radius = 6371e3 + 50e3;

    return ray_sphere(pos, normalize(dir), earth_center, atmo_radius);
}

// computes how much the given ray is occluded by the horizon (i.e. if it
// intersects the surface of the Earth then it will not contribute at all)
float horizon_extinction(vec3 pos, vec3 dir) {
    return 1.0; // TODO: implement this
}

varying vec3 pos;
uniform vec3 skycolor;
float phase(float cosangle, float c) {
	float a = 9/(2.0f*(c*c+2.0f))-3.0f/2.0f;
	float b = (1.0f*cosangle*cosangle)/(pow(1.0f+c*c-2.0f*c*cosangle,1.5));
	return a*b;
}
vec3 absorb(float dist, vec3 color, float factor){
    return color-color*pow(skycolor, vec3(factor/dist));
}
const int step_count = 32;
void main()
{
    if (pos.y < 0) {
        gl_FragColor = vec4(0, 0, 0, 1);
        return;
    }

	vec3 ray = -normalize(pos);
	vec3 light_dir = normalize(gl_LightSource[0].position.xyz);
	float dotP = max(0, dot(ray,light_dir));
	float rayleigh = phase(dotP,-0.01)*33;
	float mie = phase(dotP,-0.875)*100;
	float spot = smoothstep(0.0, 15.0, phase(dotP,0.9995))*1000;

	vec3 accumulator = vec3(0, 0, 0);

	vec3 eye_pos = vec3(0, 1.8, 0);
	float eye_atmo_dist = atmospheric_depth(eye_pos, ray);
	float step_size = eye_atmo_dist / step_count;

	vec3 eye_dir = -normalize(pos);

	vec3 mie_collected = vec3(0);
	vec3 rayleigh_collected = vec3(0);

	for(int i=0;i<step_count;i++) {
		float sample_distance = step_size*float(i);
		vec3 sample_pos = sample_distance*ray + eye_pos;

		float extinction = 1;//horizon_extinction(sample_pos,light_dir,1.8);

		float sample_depth = atmospheric_depth(sample_pos,light_dir);

		vec3 influx = absorb(sample_depth, vec3(1.8), 28)*extinction;

		rayleigh_collected += absorb(sample_distance, Kr*influx, 7900);
		mie_collected += absorb(sample_distance, influx, 16400);
	}

	float eye_extinction = 1/*horizon_extinction(eye_position, ray,surface_height - 0.3)*/;
	rayleigh_collected = (
		rayleigh_collected *
		eye_extinction *
		pow(eye_atmo_dist, 1.11)
	)/float(step_count);
	mie_collected = (
		mie_collected *
		eye_extinction *
		pow(eye_atmo_dist, 1.19)
	)/float(step_count);

	vec3 color = vec3(
		spot*mie_collected +
		mie*mie_collected +
		rayleigh*rayleigh_collected
	);
    gl_FragColor = vec4(color,1);
}
