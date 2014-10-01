#include "core/skybox.h"

skybox::skybox()
    : m_shader("skybox/skybox.vert", "skybox/skybox.frag")
{
    quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH);
}

void skybox::display(const camera& cam, glm::vec3 skycolor)
{
	float RayMult = 0; //Rayleigh scattering multipliers
	float MieMult = 0; //Mie scattering multipliers
	float InMult = 0; //Inscattering multipliers
    m_shader.bind();

    m_shader.set("view", cam.view(false));
	m_shader.set("proj", cam.proj());

	glm::vec4 altitude = glm::vec4(0);

	altitude.x = 0; //actualy supposed to by Eye->y/1000.0f but as Eye->y currently can only = 0 have simplified;
	altitude.y = std::pow(0.95f, altitude.x);
	altitude.z = 20000.0f * std::exp(0); //0 supposed to be -Eye->y/10000.0f
	altitude.w = 8000.0f * std::exp(0); //0 Supposed to be -Eye->y/20000.0f

	float g = 0.444f + cam.dir().y * 1.053e-5 - 5.984e-2 * (0) - 3.521e-6 * (0) *cam.dir().y;  //(0) = GetSunTheta;
	float g2 = 0.1f + cam.dir().y * 1.11e-5 - 3.18e-2*(0) - 6.01e-6 * (0) * cam.dir().y;  //(0) = GetSunTheta;
	float c = -0.076923f + 153.846f * MieMult;
	g = g + c*(g2 - g);
	glm::vec4 phase = glm::vec4(1-g*g,1+g*g,2*g,InMult);

	m_shader.set("altitudeDensity", altitude);
	m_shader.set("totalRayleighScattering", glm::vec3(0));
	m_shader.set("totalRayleighScatteringMult", glm::vec3(0 * 3 / (16 * 3.14))); //above * 3/16pi
	m_shader.set("totalMieScattering", glm::vec3(0));
	m_shader.set("totalMieScatteringMult", glm::vec3(0 * 1 / (4 * 3.14))); //above * 1/4pi
	glm::vec3 tmp = (glm::vec3(0 * 1 / (4 * 3.14))*glm::vec3(0 * 3 / (16 * 3.14)));
	m_shader.set("OneOverTotalRM", glm::vec3(1.0f / tmp.x, 1.0f / tmp.y, 1.0f / tmp.z));
	m_shader.set("phase", phase);
	m_shader.set("SunColorIntensity", glm::vec4(0.988f,0.831f,0.251f, 0.5f)); //SunIntensity

    //glBegin();
    gluSphere(quad, 100, 32, 32);
    //glEnd();

    m_shader.unbind();
}
