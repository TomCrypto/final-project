#version 120

varying vec3 pos;

void main()
{
	float dotP = dot(normalize(gl_LightSource[0].position.xyz),normalize(pos));

	vec4 sun = vec4(0,0,0,0);
	if(dotP>0.9875) {
		float start = 1;
		float finish = 0;

		float grad = 80*(dotP-0.9875);

		float alpha = mix(start, finish, grad);
		sun = vec4(1,1,1,alpha);
	}

    float phi = atan(pos.x, pos.z);
    float theta = acos(pos.y / length(pos));

    vec3 topColor = vec3(0, 0, 1);
    vec3 bottomColor = vec3(1, 1, 1);

    vec3 color = mix(topColor, bottomColor, pow(theta / 3.14159265 * 1.9, 5));

    gl_FragColor = vec4(1.0) * vec4(color,1) + vec4(1.0 - vec4(color,1)) * sun;
}
