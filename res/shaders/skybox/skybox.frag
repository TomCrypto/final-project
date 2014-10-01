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
