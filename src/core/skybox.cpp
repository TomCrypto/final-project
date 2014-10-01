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
    m_shader.bind();

    m_shader.set("view", cam.view(false));
	m_shader.set("proj", cam.proj());

	glm::vec4 altitude = glm::vec4(0);

	altitude.x = 0; //actualy supposed to by Eye->y/1000.0f but as Eye->y currently can only = 0 have simplified;
	altitude.y = std::pow(0.95f, altitude.x);
	altitude.z = 20000.0f * std::exp(0); //0 supposed to be -Eye->y/10000.0f
	altitude.w = 8000.0f * std::exp(0); //0 Supposed to be -Eye->y/20000.0f

	m_shader.set("altitudeDensity", altitude);
	m_shader.set("totalRayleighScattering", glm::vec3(0, 0, 0));
	m_shader.set("totalMieScattering", glm::vec3(0, 0, 0));

    //glBegin();
    gluSphere(quad, 100, 32, 32);
    //glEnd();

    m_shader.unbind();
}
