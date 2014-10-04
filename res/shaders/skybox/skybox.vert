#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 extinction;
varying vec3 inscattering;

uniform float Esun;
uniform vec3 sunDir;
uniform vec3 eyePos;
uniform vec3 betaRay;
uniform vec3 betaMie;
uniform vec3 betaRM;
uniform vec3 oneOverBetaRM;
uniform vec3 gHG;

void main()
{
gl_Position = proj * view * gl_Vertex;

extinction = gl_Vertex.xyz;
inscattering = vec3(0);
}