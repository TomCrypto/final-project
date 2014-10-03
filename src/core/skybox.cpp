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

	m_shader.set("light", vars.light);

    //glBegin();
    gluSphere(quad, 100, 32, 32);
    //glEnd();

    m_shader.unbind();
}
