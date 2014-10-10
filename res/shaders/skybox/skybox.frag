#version 120

varying vec3 extinction;
varying vec3 inscattering;
varying vec3 debug;

void main()
{
    gl_FragColor = 10 * vec4(vec3(0.0138,0.0113, 0.008) * extinction + inscattering, 1.0);
    //gl_FragColor = vec4(debug, 1.0);
}
