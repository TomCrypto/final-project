#include <easylogging.h>

#include "core/occlusion.h"

occlusion::occlusion()
    : m_tex(glm::ivec2(max_lights, resolution), GL_FLOAT),
      m_shader("occlusion/occlusion.vert", "occlusion/occlusion.frag")
{

}

occlusion::~occlusion()
{

}

const gl::texture2D& occlusion::query(const std::vector<light>& lights,
                                      framebuffer& framebuffer,
                                      const camera& camera)
{
    if (lights.size() > max_lights) {
        LOG(WARNING) << "More lights than supported! Extra ignored.";
    }

    size_t light_count = std::min(lights.size(), max_lights);

    // first grab a copy of the framebuffer
    const auto& copy = framebuffer.frame_copy();

    // next attach our occlusion texture to the framebuffer
    framebuffer.bind_as(m_tex);
    glViewport(0, 0, 8, 8);

    framebuffer.clear(false);
    glDisable(GL_DEPTH_TEST);

    // set up our occlusion shader and attach the rendered stuff to it
    m_shader.bind();
    copy.bind(0);
    m_shader.set("render", 0);
    m_shader.set("max_lights", (int)max_lights);
    //m_shader.set("resolution", (int)resolution);
    //m_shader.set("light_count", (int)light_count);
    m_shader.set("viewproj", camera.proj() * camera.view());
    //m_shader.set("view_dir", glm::normalize(camera.dir()));
    //m_shader.set("view_pos", camera.pos());
    
    for (size_t t = 0; t < light_count; ++t) {
        m_shader.set("lights[" + std::to_string(t) + "].pos",
                     lights[t].pos);
        m_shader.set("lights[" + std::to_string(t) + "].radius",
                     lights[t].radius);
    }

    // finally render a bunch of points, one for every light + radius
    glBegin(GL_POINTS);

    for (size_t i = 0; i < light_count; ++i) {
        for (size_t j = 0; j < resolution; ++j) {
            glNormal3f((float)i / max_lights,
                       (float)j / resolution,
                       0.0f);
            glVertex2f((float)i / max_lights * 2 - 1,
                       (float)j / resolution * 2 - 1);
        }
    }

    // todo
    glEnd();

    m_shader.unbind();
    framebuffer.bind();
    glEnable(GL_DEPTH_TEST);

    return m_tex;
}