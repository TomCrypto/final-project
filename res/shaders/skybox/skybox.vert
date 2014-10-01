#version 120

uniform mat4 view;
uniform mat4 proj;

uniform vec3 totalRayleighScattering;
uniform vec3 totalMieScattering;
uniform vec4 altitudeDensity;

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


	color = viewAngle.xyz;
}
