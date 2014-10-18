#version 120

// computes atmospheric scattering at a given point in the sky

varying vec3 pos;

uniform float extinction;
uniform vec3 Esun;
uniform vec3 sunDir;
uniform vec3 betaDashRay;
uniform vec3 betaDashMie;
uniform vec3 oneOverBetaRayMie;
uniform vec3 gHG; //glm::vec3((1 - g)*(1 - g), 1 + g*g, 2 * g)

void main()
{
    if (pos.y < 0) {
        gl_FragColor = vec4(0, 0.15, 0.85, 1);
    } else {
		float dotP = dot(normalize(pos),normalize(sunDir));

		vec3 ray = betaDashRay*(1+dotP*dotP);
		float hg = (gHG.x/pow(gHG.y-gHG.z*dotP,1.5))/(4*3.14159265359);
		vec3 mie = betaDashMie * hg;

		vec3 inscattering = (ray + mie)*oneOverBetaRayMie*Esun;
        gl_FragColor = vec4(inscattering / extinction, 1.0);
    }
}
