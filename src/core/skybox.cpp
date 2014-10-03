#include "core/skybox.h"

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

	
    m_shader.set("light_incl", vars.inclination);
    m_shader.set("light_lat", vars.latitude);
	m_shader.set("scatter_strength", vars.InMult);
	m_shader.set("rayleigh_strength", vars.RayMult);
	m_shader.set("mie_strength", vars.MieMult);

    //glBegin();
    gluSphere(quad, 100, 32, 32);
    //glEnd();

    m_shader.unbind();
}
