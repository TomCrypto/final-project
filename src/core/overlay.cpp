#include <easylogging.h>

#include "core/overlay.h"

#include <cstdio>

overlay::overlay(int density)
    : m_shader("overlay/overlay.vert", "overlay/overlay.frag")
{
    regenerate_film(density);
}

void overlay::regenerate_film(int density)
{
    // this is awful-fix it later!!
    srand(42);

    m_film.clear();
    
    for (int t = 0; t < density; ++t) {
        m_film.push_back(std::make_pair(
            glm::vec2((float)rand() / RAND_MAX * 2 - 1,
                      (float)rand() / RAND_MAX * 2 - 1),
            0.005f + 0.045f * (float)rand() / RAND_MAX));
    }
}

void overlay::render(const std::vector<light>& lights,
                     const camera& camera)
{
    if (lights.size() > 8) {
        LOG(WARNING) << "Maximum 8 lights! Extra lights ignored by overlay.";
    }

    /* Render a set of randomly distributed points with glBlend enabled. */
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
    m_shader.bind();
    
    m_shader.set("viewproj", camera.proj() * camera.view());
    m_shader.set("view_dir", glm::normalize(camera.dir()));
    m_shader.set("light_count", std::min((int)lights.size(), 8));
    m_shader.set("inv_ratio", 1.0f / camera.aspect_ratio());
    
    for (int t = 0; t < std::min((int)lights.size(), 8); ++t) {
        m_shader.set("lights[" + std::to_string(t) + "].pos",
                     lights[t].pos);
        m_shader.set("lights[" + std::to_string(t) + "].strength",
                     lights[t].strength);
    }
    
    glBegin(GL_QUADS);
    
    for (size_t t = 0; t < m_film.size(); ++t) {
        glm::vec2 pos = m_film[t].first;
        float radius = m_film[t].second;
    
        glTexCoord2f(0, 1);
        glVertex3f(pos.x - radius, pos.y + radius, 1);
        glTexCoord2f(1, 1);
        glVertex3f(pos.x + radius, pos.y + radius, 1);
        glTexCoord2f(1, 0);
        glVertex3f(pos.x + radius, pos.y - radius, 1);
        glTexCoord2f(0, 0);
        glVertex3f(pos.x - radius, pos.y - radius, 1);
    }

    glEnd();

    m_shader.unbind();

    glDisable(GL_BLEND);
}
