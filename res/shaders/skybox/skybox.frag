#version 120

float ray_sphere(vec3 p, vec3 d, vec3 o, float r) {
    vec3 l = o - p;
    float a = dot(d, d);
    float b = 2 * dot(d, l);
    float c = dot(l, l) - r * r;

    float discr = b * b - 4 * a * o;
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
    const vec3 atmo_radius = vec3(0, 6371e3 + 50e3);

    return ray_sphere(pos, normalize(dir), earth_center, atmo_radius);
}

// computes how much the given ray is occluded by the horizon (i.e. if it
// intersects the surface of the Earth then it will not contribute at all)
float horizon_extinction(vec3 pos, vec3 dir) {
    return 1; // TODO: implement this
}

varying vec3 pos;
float phase(float cosangle, float c) {
	float a = 9/(2.0f*(c*c+2.0f))-3.0f/2.0f;
	float b = (1.0f*cosangle*cosangle)/(pow(1.0f+c*c-2.0f*c*cosangle,1.5));
	return a*b;
}

void main()
{
	float dotP = dot(-normalize(pos),normalize(gl_LightSource[0].position.xyz));
	float rayleigh = phase(dotP,-0.01)*33;
	float mie = phase(dotP,-0.875)*100;
	float spot = smoothstep(0.0, 15.0, phase(dotP,0.9995))*1000;
	
	vec3 kr = vec3(0.18867780436772762, 0.4978442963618773, 0.6616065586417131);



	vec3 mie_collected = vec3(0.25,0.25,0.25);
	vec3 rayleigh_collected = kr;
	vec3 color = vec3(
		spot*mie_collected +
		mie*mie_collected +
		rayleigh*rayleigh_collected
	);
    gl_FragColor = vec4(color,1);
}
