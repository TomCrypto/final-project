#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 extinction;
varying vec3 inscattering;

uniform vec4 Esun;
uniform vec3 sunDir;
uniform vec3 eyePos;
uniform vec3 betaRay;
uniform vec3 rayleighTheta;
uniform vec3 betaMie;
uniform vec3 mieTheta;
uniform vec3 betaRM;
uniform vec3 oneOverBetaRM;
uniform vec3 gHG; //glm::vec3((1 - g)*(1 - g), 1 + g*g, 2 * g)

void main()
{
gl_Position = proj * view * gl_Vertex;
vec3 eyeDir = normalize((gl_Vertex).xyz);
float dotP = -dot(normalize(eyeDir),normalize(sunDir));

if (dotP < 0.995) {
	extinction = vec3(0);
	inscattering = vec3(0);
	return;
} else {
	extinction = vec3(10000);
	inscattering = vec3(00000);
	return;
}

extinction = (dotP/2.0f + 0.5f)*vec3(1.0f);//exp(-betaRM*gl_Position.z);
vec3 ray = rayleighTheta*(1+dotP*dotP);
float hg = gHG.x/pow(gHG.y-gHG.z*dotP,1.5f);
vec3 mie = mieTheta*hg;
//tmp = tmp + mieTheta*dotP;
//tmp = tmp * oneOverBetaRM * Esun.xyz * (1-extinction); //still need to multiply by Esun
inscattering = ray+mie* oneOverBetaRM * (1-extinction);//(ray+mie)* oneOverBetaRM;
}