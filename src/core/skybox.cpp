#include "core/skybox.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

skybox::skybox()
    : m_shader("skybox/skybox.vert", "skybox/skybox.frag")
{
    quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH);
}

void skybox::display(const camera& cam, atmos vars)
{
    m_shader.bind();

    m_shader.set("view", cam.view(false));
	m_shader.set("proj", cam.proj());
	glm::vec3 eyePos = cam.dir();
	m_shader.set("eyePos", eyePos);

	float pi = 3.14159265358979323846f;
	float n = .0003f; //regractive index of air
	float N = 2.545e25; //molecules per unit valume of air
	float pn = 0.035f; //depolarization factor for air
	glm::vec3 lambda = glm::vec3(1 / 650e-9f, 1 / 570e-9f, 1 / 475e-9f); // red, green & blue. Note: 650e-9 m = 650nm.
	float tmp = ((6 + 3 * pn) / (6 - 7 * pn)) * ((pi*pi*std::powf((n*n) - 1.0f, 2)) / 2 * N);
	glm::vec3 rayleighTheta = tmp / (lambda*lambda*lambda*lambda);
	glm::vec3 betaRayleigh = rayleighTheta * glm::vec3(16 * pi / 3); //total BetaR
	/*std::cout << glm::to_string(rayleighMultipier) << "\n";
	std::cout << glm::to_string(betaRayleigh) << "\n";
	std::cout << glm::to_string(lambda) << "\n";
	std::cout << glm::to_string(lambda*lambda) << "\n";
	std::cout << glm::to_string(lambda*lambda*lambda*lambda) << "\n";
	std::cout << glm::to_string(glm::vec3(1,2,3) * glm::vec3(6,7,8)) << "\n";*/
	//exit(0);

	float T = 2.0f; //turbidity
	float c = (0.6544*T - 0.6510)*1e-16; //concentration factor
	float v = 4; //Junge's exponent = 4 for the sky model

	glm::vec3 K = glm::vec3(0.685f, 0.679f, 0.670f);
	float Temp = 0.434*c*(2 * pi)*(2 * pi)*0.5f;
	glm::vec3 mieTheta = Temp*lambda*lambda;
	Temp = 0.434f*c*pi*(2 * pi)*(2 * pi);;
	glm::vec3 betaMie = Temp*K*lambda*lambda;

	//Rayleigh + Mie
	glm::vec3 betaRM = betaRayleigh+betaMie;
	glm::vec3 oneOverBetaRM = glm::vec3(1) / betaRM;

	float g = 0.8f; //Henyey Greensteins's G value
	glm::vec3 HG = glm::vec3((1 - g)*(1 - g), 1 + g*g, 2 * g);



	m_shader.set("betaRay", betaRayleigh);
	m_shader.set("rayleighTheta", rayleighTheta);
	m_shader.set("betaMie", betaMie);
	m_shader.set("mieTheta", mieTheta);
	m_shader.set("betaRM", betaRM);
	m_shader.set("oneOverBetaRM", oneOverBetaRM);
	m_shader.set("gHG", HG);

	//sunDir
	glm::vec3 sunDir = glm::vec3(glm::cos(glm::radians(vars.theta)), glm::sin(glm::radians(vars.theta))*glm::cos(glm::radians(vars.phi)), glm::sin(glm::radians(vars.theta))*glm::sin(glm::radians(vars.phi)));
	m_shader.set("sunDir",sunDir);
	//calculate colour
	float fBeta = 0.04608365822050f * T - 0.04586025928522f;
	float m = 1.0f / (glm::cos(glm::radians(vars.theta)) + 0.5f/glm::pow(93.885f - vars.theta, 1.253f));
	glm::vec3 lam = glm::vec3(0.65f, 0.57f, 0.475f); //red green & blue in um
	float fTauRx = glm::exp(-m*0.008735f*glm::pow(lam.x, -4.08f));
	float fTauRy = glm::exp(-m*0.008735f*glm::pow(lam.y, -4.08f));
	float fTauRz = glm::exp(-m*0.008735f*glm::pow(lam.z, -4.08f));
	float fTauAx = glm::exp(-m*fBeta*glm::pow(lam.x, -1.3f));
	float fTauAy = glm::exp(-m*fBeta*glm::pow(lam.y, -1.3f));
	float fTauAz = glm::exp(-m*fBeta*glm::pow(lam.z, -1.3f));
	glm::vec4 sunColorAndIntensity = glm::vec4(fTauRx*fTauAx, fTauRy*fTauAy, fTauRz*fTauAz, 1.0f);
	/*std::cout << fTauRx << " " << fTauRy << " " << fTauRz << "\n";
	std::cout << fTauAx << " " << fTauAy << " " << fTauAz << "\n";
	std::cout << glm::to_string(sunColorAndIntensity) << "\n";
	std::cout << glm::to_string(lam) << "\n";
	std::cout << m << "\n";
	exit(0);*/
	m_shader.set("Esun", sunColorAndIntensity);
	/*
	
	uniform float Esun;
	uniform vec3 sunDir;
	uniform vec3 eyePos;
	uniform vec3 betaRay;
	uniform vec3 betaMie;
	uniform vec3 betaRM;
	uniform vec3 oneOverBetaRM;
	uniform vec3 gHG;
	
	*/
    /*m_shader.set("light_incl", vars.inclination);
    m_shader.set("light_lat", vars.latitude);
	m_shader.set("scatter_strength", vars.InMult);
	m_shader.set("rayleigh_strength", vars.RayMult);
	m_shader.set("mie_strength", vars.MieMult);*/

    //glBegin();
    gluSphere(quad, 100, 32, 32);
    //glEnd();

    m_shader.unbind();
}
