#version 120

// passthrough of the vertex shader output to the render target

varying vec3 total_occlusion;

void main()
{
	gl_FragColor = vec4(total_occlusion, 1.0);
}
