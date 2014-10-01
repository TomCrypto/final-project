#version 120

uniform mat4 view;
uniform mat4 proj;

uniform vec3 totalRayleighScattering;
uniform vec3 totalRayleighScatteringMult;
uniform vec3 totalMieScattering;
uniform vec3 totalMieScatteringMult;
uniform vec3 OneOverTotalRM;
uniform vec4 altitudeDensity;
uniform vec4 phase;
uniform vec4 SunColorIntensity;

varying vec3 color;

void main()
{

    gl_Position = proj * view * gl_Vertex;

	vec4 viewAngle = vec4(0);
	viewAngle.x = dot(gl_LightSource[0].position.xyz,gl_Normal);
	viewAngle.y = (viewAngle.x*viewAngle.x)/2 + 2;
	viewAngle.z = mix(gl_MultiTexCoord0.y, altitudeDensity.z, gl_MultiTexCoord0.x);
	viewAngle.w = mix(gl_MultiTexCoord0.y, altitudeDensity.w, gl_MultiTexCoord0.x);
	
	vec3 extinction = totalRayleighScattering*viewAngle.z + totalMieScattering*viewAngle.w;
	extinction = exp(-extinction);

	//calculate mie scattering term
	vec4 phaseMie = vec4(0);
	phaseMie.x = phase.z*viewAngle.x+phase.y;
	phaseMie.y = inversesqrt(phaseMie.x);
	phaseMie.z = pow(phaseMie.y,3);
	phaseMie.w = phaseMie.z*phase.x;

	vec3 rayleigh = totalRayleighScatteringMult*viewAngle.y;
	vec3 mie = totalMieScatteringMult*phaseMie.w; 
	vec3 tmp = 1.0f - extinction;

	vec3 inscatter = (mie+rayleigh)*OneOverTotalRM;
	inscatter *= tmp;
	inscatter *= SunColorIntensity.xyz;
	inscatter *= SunColorIntensity.w;

	inscatter.z += 0.15f;
	color = inscatter;
}
