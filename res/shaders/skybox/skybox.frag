#version 120

varying vec3 extinction;
varying vec3 inscattering;
varying vec3 debug;

void main()
{
    gl_FragColor = vec4(inscattering / vec3(0.0138, 0.0113, 0.008), 1.0);
}
