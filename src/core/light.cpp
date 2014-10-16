#include "core/light.h"

light_renderer::light_renderer() :
    m_shader("light.vert", "light.frag")
{
    m_quad = gluNewQuadric();
}

light_renderer::~light_renderer()
{
    gluDeleteQuadric(m_quad);
}

void light_renderer::display(const camera& camera,
                             const std::vector<light>& lights,
                             const glm::mat4& transform)
{
    m_shader.bind();
    m_shader.set("proj", camera.proj());

    for (auto light : lights) {
        bool fixed = (light.position.w == 0);
        m_shader.set("view", camera.view(!fixed));
        m_shader.set("light_color", light.intensity);
        m_shader.set("light_pos", (glm::vec3)(transform * light.position));

        if (fixed) {
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
        }

        gluSphere(m_quad, light.radius, 16, 16);
    }

    m_shader.unbind();

    glEnable(GL_DEPTH_TEST);
}
