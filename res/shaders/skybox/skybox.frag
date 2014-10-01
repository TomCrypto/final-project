#version 120

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
	float mie = phase(dotP,-0.01)*33;
	vec4 sun = vec4(0,0,0,0);
	if(dotP>0.99375) {
		vec3 topColor1 = vec3(1, 0, 0);
		vec3 bottomColor1 = vec3(1, 1, 0);

		float grad = 160*(dotP-0.99375);

		sun = vec4(mix(topColor1, bottomColor1, grad),0.25);
	}

    float phi = atan(pos.x, pos.z);
    float theta = acos(pos.y / length(pos));

    vec3 topColor = vec3(0, 0, 1);
    vec3 bottomColor = vec3(1, 1, 1);

    vec3 color = mix(topColor, bottomColor, pow(theta / 3.14159265 * 1.9, 5));

    gl_FragColor = vec4(1.0) * vec4(color,1) + vec4(1.0 - vec4(color,1)) * sun;
}
