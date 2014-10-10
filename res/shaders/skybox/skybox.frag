#version 120

varying vec3 extinction;
varying vec3 inscattering;

void main()
{
gl_FragColor = vec4(inscattering,1);
}
