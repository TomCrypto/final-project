#include <easylogging.h>

#include "core/skybox.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cmath>

skybox::skybox()
    : m_shader("skybox.vert", "skybox.frag")
{
    quad = gluNewQuadric();
}

skybox::~skybox()
{
    gluDeleteQuadric(quad);
}

glm::vec3 skybox::calcSunColor(float timeofday, float T) {
    timeofday = (timeofday - 6) / 12;

	float theta = 87.0 - 45.0*glm::sin(timeofday*3.14159265358979323846f);
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

glm::vec3 skybox::calcSunDir(float timeofday) {
    timeofday = (timeofday - 6) / 12;

	float theta = 87.0 - 45.0*glm::sin(timeofday*3.14159265358979323846f);
	float phi = 22.5 + timeofday*(157.5 - 22.5);
	return glm::vec3(glm::sin(glm::radians(theta))*glm::cos(glm::radians(phi)),
		glm::cos(glm::radians(theta)),
		glm::sin(glm::radians(theta))*glm::sin(glm::radians(phi)));
}

void skybox::display(const camera& cam, atmos vars, const std::vector<light>& lights)
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

	float g = 0.8f; //Henyey Greensteins's G value
	glm::vec3 HG = glm::vec3((1 - g)*(1 - g), 1 + g*g, 2 * g);

	m_shader.set("gHG", HG);

	//sunDir
	m_shader.set("sunDir",calcSunDir(vars.timeofday));
	//calculate colour
	m_shader.set("Esun", vars.sunColor);

    gluSphere(quad, 100, 4, 4);

    glEnable(GL_DEPTH_TEST);
}
