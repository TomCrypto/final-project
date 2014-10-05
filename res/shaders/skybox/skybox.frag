#version 120

varying vec3 extinction;
varying vec3 inscattering;

void main()
{
gl_FragColor = vec4(0.0138f,0.0113f, 0.008f,1)*vec4(extinction,1)+vec4(inscattering,1);
}
