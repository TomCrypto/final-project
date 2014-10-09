#include <easylogging.h>

#include "core/overlay.h"

#include <cstdio>
#include <cmath>

static bool has_space(const glm::vec2& point,
                      const std::vector<glm::vec2>& points,
                      float min_dt) {
    for (auto p : points) {
        if (glm::length(point - p) < min_dt) {
            return false;
        }
    }

    return true;
}

/* Generates n random points in [-1, 1] spaced roughly min_dt apart. */
static std::vector<glm::vec2> gen_points(size_t n, float min_dt) {
    std::vector<glm::vec2> points;

    while (n > 0) {
        auto point = glm::vec2((float)rand() / RAND_MAX * 2 - 1,
                               (float)rand() / RAND_MAX * 2 - 1);

        if (has_space(point, points, min_dt)) {
            points.push_back(point);
            --n;
        }
    }

    return points;
}

/* Returns a randomly selected disk radius with good distribution. */
static float disk_radius() {
    float u = (float)rand() / RAND_MAX;
    return 0.005f + sqrt(u) * 0.065f;
}

overlay::overlay(int density)
    : m_shader("overlay/overlay.vert", "overlay/overlay.frag")
{
    regenerate_film(density);
}

void overlay::regenerate_film(int density) {
    m_film.clear();

    srand(42);
    auto points = gen_points(density, 0.1f);
    srand(42);

    for (auto point : points) {
        m_film.push_back(std::make_pair(point, disk_radius()));
    }
}

void overlay::render(const std::vector<light>& lights,
                     const gl::texture2D& occlusion,
                     const camera& camera,
                     float reflectivity) {
    if (lights.size() > 8) {
        LOG(WARNING) << "Maximum 8 lights! Extra lights ignored by overlay.";
    }

    /* Render a set of randomly distributed points with glBlend enabled. */

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glViewport(0, 0, camera.dims().x, camera.dims().y);

    m_shader.bind();

    m_shader.set("viewproj", camera.proj() * camera.view());
    m_shader.set("view_dir", glm::normalize(camera.dir()));
    m_shader.set("view_pos", camera.pos());
    m_shader.set("light_count", std::min((int)lights.size(), 8));
    m_shader.set("inv_ratio", 1.0f / camera.aspect_ratio());
    m_shader.set("reflectivity", reflectivity);

    occlusion.bind(0, GL_NEAREST, GL_NEAREST);
    m_shader.set("occlusion", 0);
    m_shader.set("max_lights", 8);

    for (int t = 0; t < std::min((int)lights.size(), 8); ++t) {
        m_shader.set("lights[" + std::to_string(t) + "].pos",
                     lights[t].pos);
    }

    glBegin(GL_QUADS);

    for (size_t t = 0; t < m_film.size(); ++t) {
        glm::vec2 pos = m_film[t].first;
        float radius = m_film[t].second;

        glTexCoord2f(0, 1);
        glVertex3f(pos.x * camera.aspect_ratio() - radius, pos.y + radius, 1);
        glTexCoord2f(1, 1);
        glVertex3f(pos.x * camera.aspect_ratio() + radius, pos.y + radius, 1);
        glTexCoord2f(1, 0);
        glVertex3f(pos.x * camera.aspect_ratio() + radius, pos.y - radius, 1);
        glTexCoord2f(0, 0);
        glVertex3f(pos.x * camera.aspect_ratio() - radius, pos.y - radius, 1);
    }

    glEnd();

    m_shader.unbind();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
