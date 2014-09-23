#include "core/skybox.h"

skybox::skybox()
    : m_shader("skybox/skybox.vert", "skybox/skybox.frag")
{
    quad = gluNewQuadric();
}

void skybox::display(const camera& cam)
{
    m_shader.bind();

    m_shader.set("view", cam.view());
    m_shader.set("proj", cam.proj());

    //glBegin();
    gluSphere(quad, 100, 10, 10);
    //glEnd();

    m_shader.unbind();
}
