#include <easylogging.h>

#include "core/skybox.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cmath>

skybox::skybox()
    : m_shader("skybox.vert", "skybox.frag"),
      m_sun("sun.vert", "sun.frag")
{
    quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH);
}

glm::vec3 skybox::calcSunColor(float theta, float T) {
	float fBeta = 0.04608365822050f * T - 0.04586025928522f;
	float m = 1.0f / (glm::cos(glm::radians(theta)) + 0.15f * std::pow(93.885f - theta, -1.253f));
	glm::vec3 lam = glm::vec3(0.65f, 0.57f, 0.475f); //red green & blue in um
	float fTauRx = glm::exp(-m*0.008735f*std::pow(lam.x, -4.08f));
	float fTauRy = glm::exp(-m*0.008735f*std::pow(lam.y, -4.08f));
	float fTauRz = glm::exp(-m*0.008735f*std::pow(lam.z, -4.08f));
	float fTauAx = glm::exp(-m*fBeta*std::pow(lam.x, -1.3f));
	float fTauAy = glm::exp(-m*fBeta*std::pow(lam.y, -1.3f));
	float fTauAz = glm::exp(-m*fBeta*std::pow(lam.z, -1.3f));
	return glm::vec3(fTauRx*fTauAx, fTauRy*fTauAy, fTauRz*fTauAz);
}

glm::vec3 skybox::calcSunDir(float theta, float phi) {
	return glm::vec3(glm::sin(glm::radians(theta))*glm::cos(glm::radians(phi)),
		glm::cos(glm::radians(theta)),
		glm::sin(glm::radians(theta))*glm::sin(glm::radians(phi)));
}

void skybox::display(const camera& cam, atmos vars)
{
    glViewport(0, 0, cam.dims().x, cam.dims().y);

    glDisable(GL_DEPTH_TEST);

    m_shader.bind();

    m_shader.set("view", cam.view(false));
	m_shader.set("proj", cam.proj());

	m_shader.set("extinction", vars.extinction);

	float pi = 3.14159265358979323846f;
	float n = 1.0003f; //regractive index of air
	float N = 2.545e25; //molecules per unit valume of air
	float pn = 0.035f; //depolarization factor for air
	glm::vec3 lambda = glm::vec3(1 / 650e-9f, 1 / 570e-9f, 1 / 475e-9f); // red, green & blue. Note: 650e-9 m = 650nm.

	float tmp = pi*pi*(n*n - 1.0f)*(n*n - 1.0f)*(6+3*pn)/(6-7*pn)/N;
	glm::vec3 betaRay = (8.0f*tmp*pi / 3) * lambda*lambda*lambda*lambda;
	glm::vec3 rayleighTheta = (tmp/2) * (lambda*lambda*lambda*lambda);
	m_shader.set("betaDashRay", rayleighTheta*vars.ray);

	float c = (0.6544*vars.turbidity - 0.6510)*1e-16; //concentration factor
	tmp = 0.434*c*(2 * pi)*(2 * pi)*0.5f;
	glm::vec3 mieTheta = tmp*lambda*lambda;
	m_shader.set("betaDashMie", mieTheta*vars.mie);

	glm::vec3 K = glm::vec3(0.685f, 0.679f, 0.670f);
	tmp = 0.434f*c*pi*(2 * pi)*(2 * pi);
	glm::vec3 betaMie = tmp*K*lambda*lambda;

	glm::vec3 betaRayMie = betaRay + betaMie;
	glm::vec3 oneOverBetaRayMie = 1.0f / (betaRayMie);
	m_shader.set("oneOverBetaRayMie", oneOverBetaRayMie);
	/*LOG(ERROR) << glm::to_string(glm::vec3(lambda*lambda*lambda*lambda));
	LOG(ERROR) << glm::to_string(rayleighTheta*glm::vec3(vars.ray));
	LOG(ERROR) << glm::to_string(rayleighTheta*glm::vec3(50));
	exit(1);*/
	//total BetaR

	/*std::cout << glm::to_string(rayleighMultipier) << "\n";
	std::cout << glm::to_string(betaRayleigh) << "\n";
	std::cout << glm::to_string(lambda) << "\n";
	std::cout << glm::to_string(lambda*lambda) << "\n";
	std::cout << glm::to_string(lambda*lambda*lambda*lambda) << "\n";
	std::cout << glm::to_string(glm::vec3(1,2,3) * glm::vec3(6,7,8)) << "\n";*/
	//exit(0);





	//Rayleigh + Mie
	//glm::vec3 betaRM = betaRayleigh+betaMie;
	//glm::vec3 oneOverBetaRM = glm::vec3(1) / betaRM;

	float g = 0.8f; //Henyey Greensteins's G value
	glm::vec3 HG = glm::vec3((1 - g)*(1 - g), 1 + g*g, 2 * g);

	//m_shader.set("betaRM", betaRM);
	//m_shader.set("oneOverBetaRM", oneOverBetaRM);
	m_shader.set("gHG", HG);

	//sunDir
	m_shader.set("sunDir",calcSunDir(vars.theta,vars.phi));
	//calculate colour
	m_shader.set("Esun", vars.sunColor);
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


	glPushMatrix();
    gluSphere(quad, 100, 256, 256);

    m_sun.bind();
    m_sun.set("view", cam.view(false));
    m_sun.set("proj", cam.proj());

    m_sun.set("sun_color", vars.sunBrightness * calcSunColor(vars.theta, vars.turbidity));
    m_sun.set("sun_pos", calcSunDir(vars.theta, vars.phi));

    float sun_radius = 0.02f;

    gluSphere(quad, sun_radius, 64, 64);

    m_sun.unbind();
	glPopMatrix();

    glEnable(GL_DEPTH_TEST);
}
