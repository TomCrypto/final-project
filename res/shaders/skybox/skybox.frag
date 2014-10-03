#version 120

float ray_sphere(vec3 p, vec3 d, vec3 o, float r) {
vec3 l = o - p;
float a = dot(d, d);
float b = 2 * dot(d, l);
float c = dot(l, l) - r * r;

float discr = b * b - 4 * a * c;
// assert discr >= 0

return (-b + sqrt(discr)) / (2 * a);
}

// computes the distance in meters from the position to the edge of the atmosphere
// assuming the earth is at (0, -6371km, 0) and the atmosphere has height 50km
//
// assumes the point is between the earth's surface and atmosphere
// dir need not be normalized
float atmospheric_depth(vec3 position, vec3 dir) {
float a = dot(dir, dir);
    float b = 2.0*dot(dir, position);
    float c = dot(position, position)-1.0;
    float det = b*b-4.0*a*c;
    float detSqrt = sqrt(det);
    float q = (-b - detSqrt)/2.0;
    float t1 = c/q;
    return t1;
}

// computes how much the given ray is occluded by the horizon (i.e. if it
// intersects the surface of the Earth then it will not contribute at all)
float horizon_extinction(vec3 pos, vec3 dir) {
return 1.0; // TODO: implement this
}

varying vec3 norm;
uniform float scatter_strength;
uniform float rayleigh_strength;
uniform float mie_strength;
uniform float light_incl;
uniform float light_lat;
float phase(float cosangle, float c) {
float a = (3.0*(1.0-c*c))/(2.0*(2.0+c*c));
float b = (1.0f*cosangle*cosangle)/(pow(1.0f+c*c-2.0f*c*cosangle,1.5));
return a*b;
}

vec3 Kr = vec3(
    0.18867780436772762, 0.4978442963618773, 0.6616065586417131
);

vec3 absorb(float dist, vec3 color, float factor){
    return color-color*pow(Kr, vec3(factor/dist));
}

void main()
{
/*if (norm.y < 0) {
        gl_FragColor = vec4(0, 0, 0, 1);
        return;
    }*/

vec3 light = normalize(vec3(-cos(radians(-light_incl)),
                            sin(radians(-light_incl)),
                            light_lat / 90));

float dotP = dot(normalize(norm),normalize(light));
float rayleigh = phase(dotP,-0.01)*33;
float mie = phase(dotP,-0.875)*10;
float spot = smoothstep(0.0, 15.0, phase(-dotP,0.9995))*1000;

vec3 kr = vec3(0.18867780436772762, 0.4978442963618773, 0.6616065586417131);


vec3 mie_collected = vec3(0);
vec3 rayleigh_collected = vec3(0);

float total_length = atmospheric_depth(vec3(0.01), normalize(norm));
float step_length = total_length / 16.0;

float intensity = 0.8;

for(int i=0; i<16; i++){
    float sample_distance = step_length*float(i);
    vec3 position = vec3(0.01) + normalize(norm)*sample_distance;
    float extinction = 1.0;
    float sample_depth = atmospheric_depth(position, light);

    vec3 influx = absorb(sample_depth, vec3(intensity), scatter_strength)*extinction;

    rayleigh_collected += influx * absorb(sample_distance, Kr, rayleigh_strength);
    mie_collected += influx * absorb(sample_distance, Kr, mie_strength);
}

vec3 color = vec3(
spot*mie_collected +
mie*mie_collected +
rayleigh*rayleigh_collected
);
gl_FragColor = vec4(color,1);
}
