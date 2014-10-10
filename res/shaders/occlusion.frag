#version 120

varying vec3 total_occlusion;

void main()
{
	gl_FragColor = vec4(total_occlusion, 1.0);
}
