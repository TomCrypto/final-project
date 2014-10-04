#version 120

varying vec3 extinction;
varying vec3 inscattering;

void main()
{
gl_FragColor = vec4(1)*vec4(extinction,1)+vec4(inscattering,1);
}
