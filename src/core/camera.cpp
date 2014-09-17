#include "core/camera.h"

#include <cmath>

camera::camera(int width, int height, float fov,
               const glm::vec3& position,
               const glm::vec3& direction)
    : m_width(width), m_height(height),
      m_fov(fov), m_pos(position),
      m_dir(direction)
{

}

void camera::resize(int width, int height)
{
    m_width = width;
    m_height = height;
}

void camera::move(const glm::vec3& direction)
{
    m_pos += glm::vec3(glm::vec4(direction, 0) * view());
}

void camera::turn(const glm::vec2& delta)
{
    float phi = atan2(m_dir.z, m_dir.x);
    float theta = acos(m_dir.y);

    phi += delta.x;
    theta += delta.y;

    if (theta > glm::pi<float>() - 1e-1f)
        theta = glm::pi<float>() - 1e-1f;
    if (theta < 1e-1f) theta = 1e-1f;

    m_dir = glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
}

glm::mat4 camera::view() const
{
    return glm::lookAt(m_pos, m_pos + m_dir, glm::vec3(0, 1, 0));
}

glm::mat4 camera::proj() const
{
    const float near_plane = 0.1f;
    const float far_plane = 1000.0f;

    return glm::perspectiveFov(m_fov, (float)m_width, (float)m_height,
                               near_plane, far_plane);
}
