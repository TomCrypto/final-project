#version 120

uniform mat4 view;
uniform mat4 proj;

varying vec3 extinction;
varying vec3 inscattering;

uniform vec3 Esun;
uniform vec3 sunDir;
uniform vec3 eyePos;
uniform vec3 betaDashRay;
uniform vec3 betaDashMie;
uniform vec3 oneOverBetaRayMie;
uniform vec3 gHG; //glm::vec3((1 - g)*(1 - g), 1 + g*g, 2 * g)

void main()
{
gl_Position = proj * view * gl_Vertex;
vec3 eyeDir = normalize((view * gl_Position).xyz);
float dotP = dot(normalize(eyeDir),normalize(-sunDir));

vec3 ray = betaDashRay*(1+dotP*dotP);
float hg = gHG.x/pow(gHG.y-gHG.z*dotP,1.5);
vec3 mie = betaDashMie * hg;

inscattering = (ray + mie)*oneOverBetaRayMie*Esun;
}
